#include "splicestates.h"
#include "scte30messages.h"
#include "channelconfig.h"
#include "splicesessions.h"
#include "channellogger.h"
#include <QDebug>
#include <QMetaEnum>
#include <QDateTime>
#include <QTextStream>

/**************************************************************************
 * CSpliceState
 **************************************************************************/

CAdState::CAdState(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent):
  QObject(parent),
  m_channelConfig(channelConfig),
  m_spliceSessions(spliceSessions),
  m_channelLogger(channelLogger),
  m_stateIsActive(false)
{
}

/** pretty print a SpliceState
 *
 * @param dbg
 * @param state
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QString CAdState::toString(AdState_t state)
{
  int enumIndex = CAdState::staticMetaObject.indexOfEnumerator("AdState_t");
  QMetaEnum metaEnum = CAdState::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(state);
}

/**************************************************************************
 * CStateNone
 **************************************************************************/

/** Process a message
 *
 * @param Message
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CStateNone::processMessage(CBaseMessage* Message)
{
  m_channelLogger->log(CLogLine::Error, QString("Message received in state None: %1").arg(Message->getMessageId()));
  return false;
}

/**************************************************************************
 * CStateInit
 **************************************************************************/

/** Process a message
 *
 * @param Message
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CStateInit::processMessage(CBaseMessage* message)
{
  switch (message->getMessageId())
  {
  case CBaseMessage::MID_InitResponse:
    if (message->getResult() != CBaseMessage::MR_SuccessfulResponse)
    {
      m_channelLogger->log(CLogLine::Warning, QString("received an InitResponse with result: %1").arg(CBaseMessage::toString(message->getResult())));
      changeState(StateNone);
    }
    else
    {
      changeState(StateOnMain);
    }
    return true;
  default:
    m_channelLogger->log(CLogLine::Error, QString("received unexpected message %1").arg(message->getMessageId()));
    changeState(StateNone);
    return false;
  }
}

void CStateInit::enterState()
{
  CAdState::enterState();
  CInitRequestMessage initRequest;
  initRequest.setChannelName(m_channelConfig->getChannelName());
  initRequest.setSplicerName(m_channelConfig->getSplicerName());
  initRequest.setChassis(m_channelConfig->m_chassis);
  initRequest.setCard(m_channelConfig->m_card);
  initRequest.setPort(m_channelConfig->m_port);
  sendMessage(&initRequest);
}


/**************************************************************************
 * CStateOnMain
 **************************************************************************/

CStateOnMain::CStateOnMain(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent):
  CAdState(channelConfig, spliceSessions, channelLogger, parent)
{
  m_aliveTimer.setInterval(60000);
  m_aliveTimer.setSingleShot(false);
  connect(&m_aliveTimer, SIGNAL(timeout()), this, SLOT(sendAliveMessage()));
}
/** Process a message
 *
 * @param Message
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CStateOnMain::processMessage(CBaseMessage* message)
{
  switch (message->getMessageId())
  {
  case CBaseMessage::MID_CueRequest:
    {
      CCueRequestMessage* msg = dynamic_cast<CCueRequestMessage*>(message);
      Q_ASSERT(msg);
      return processCueRequest(*msg);
    }
  case CBaseMessage::MID_GeneralResponse:
    return true;
  case CBaseMessage::MID_AliveResponse:
    {
      CAliveResponseMessage* msg = dynamic_cast<CAliveResponseMessage*>(message);
      Q_ASSERT(msg);
      return processAliveResponse(*msg);
    }
  case CBaseMessage::MID_SpliceResponse:
    {
      CSpliceResponseMessage* msg = dynamic_cast<CSpliceResponseMessage*>(message);
      Q_ASSERT(msg);
      return processSpliceResponse(*msg);
    }
  case CBaseMessage::MID_SpliceCompleteResponse:
    {
      CSpliceCompleteResponseMessage* msg = dynamic_cast<CSpliceCompleteResponseMessage*>(message);
      Q_ASSERT(msg);
      return processSpliceCompleteResponse(*msg);
    }
  default:
    m_channelLogger->log(CLogLine::Error, QString("received unexpected message %1").arg(message->getMessageId()));
    return false;
  }
}

void CStateOnMain::sendAliveMessage()
{
  CAliveRequestMessage aliveMessage;
  sendMessage(&aliveMessage);
}


bool CStateOnMain::processCueRequest(const CCueRequestMessage& cueRequest)
{
  //QDateTime time = cueRequest.m_time.toDateTime();
  //QDateTime now = QDateTime::currentDateTime().toUTC();
  //int timeToSplice = now.secsTo(time);
  //int timeToSpliceMs = abs(now.time().msec() - time.time().msec());

  CCueResponseMessage cueResponse;
  sendMessage(&cueResponse);
  if (cueRequest.m_spliceInfoSection.m_spliceCommandType == CSpliceInfoSection::SpliceInsert)
  {
    if (m_channelConfig->m_processScte35)
    {
      sendSpliceRequest(cueRequest);
    }
  }
  else
  {
    m_channelLogger->log(CLogLine::Error, QString("Unknown splice command type: %1").arg(CSpliceInfoSection::toString(cueRequest.m_spliceInfoSection.m_spliceCommandType)));
  }
  return true;
}

bool CStateOnMain::processAliveResponse(const CAliveResponseMessage& aliveResponse)
{
  if (aliveResponse.m_aliveState != CAliveResponseMessage::OnPrimary)
  {
    m_channelLogger->log(CLogLine::Error, "The channel is not on main! In another Ad Server running on this channel?");
  }
  int diff = QDateTime().currentDateTime().secsTo(aliveResponse.m_time.toDateTime());
  if (abs(diff) > 2)
  {
    m_channelLogger->log(CLogLine::Error, QString("The time difference between the Ad Server and the Splicer is %1s").arg(diff));
  }
  return true;
}

bool CStateOnMain::processSpliceResponse(const CSpliceResponseMessage& msg)
{
if (msg.getResult() != CBaseMessage::MR_SuccessfulResponse)
{
  m_channelLogger->log(CLogLine::Warning, QString("Received SpliceResponse with result: %1").arg(CBaseMessage::toString(msg.getResult())));
  return false;
}
return true;
}


void CStateOnMain::sendSpliceRequest(const CCueRequestMessage& cueRequest)
{
  if (cueRequest.m_spliceInfoSection.m_spliceInsert.m_outOfNetworkIndicator == 1)
  {
    CSpliceRequestMessage msg;
    if (m_channelConfig->m_adBlocks.empty() || m_channelConfig->m_adBlocks.front().m_adConfigs.empty())
    {
      m_channelLogger->log(CLogLine::Error, QString("No ads are defined"));
      return;
    }
    // Get the first Ad from the first AdBlock
    const CAdConfig& adConfig = m_channelConfig->m_adBlocks.front().m_adConfigs.front();
    m_channelConfig->m_adBlocks.front().setScte35StartTime(cueRequest.m_time.toDateTime());
    CSpliceSession* spliceSession = m_spliceSessions->createSpliceSession(cueRequest.m_time.toDateTime());
    spliceSession->setExpectedDuration(adConfig.m_duration);
    spliceSession->setNewState(CSpliceSession::Initializing);
    msg.m_sessionId = spliceSession->getSessionId();
    msg.m_priorSession = 0xFFFFFFFF;
    msg.m_time = cueRequest.m_time;
    msg.m_serviceId = adConfig.m_serviceId;
    msg.m_pcrPid = adConfig.m_pcrPid;
    msg.m_spliceElementaryStreams = adConfig.m_spliceElementaryStreams;
    msg.m_duration = adConfig.m_duration;
    msg.m_spliceEventId = cueRequest.m_spliceInfoSection.m_spliceInsert.m_spliceEventId;
    msg.m_postBlack = adConfig.m_postBlack;
    msg.m_accessType = adConfig.m_accessType;
    msg.m_overridePlaying = adConfig.m_overridePlaying;
    msg.m_returnToPriorChannel = adConfig.m_overridePlaying;
    vector<uint8_t> data(4,0);  // bitrate rule and limit
    msg.m_spliceApiDescriptors.push_back(CSpliceApiDescriptor(0x1, data));
    sendMessage(&msg);
  }
}

void CStateOnMain::sendSpliceRequest()
{

}

void CStateOnMain::enterState()
{
  CAdState::enterState();
  for (QList<CAdBlock>::iterator it = m_channelConfig->m_adBlocks.begin(); it != m_channelConfig->m_adBlocks.end(); ++it)
  {
    CAdBlock* adBlock = &(*it);
    connect(adBlock, SIGNAL(startAd(CAdConfig,QDateTime)), this, SLOT(startAd(CAdConfig,QDateTime)), Qt::UniqueConnection);
  }
  m_channelConfig->startTimers();
  m_aliveTimer.start();
  sendAliveMessage();
}

bool CStateOnMain::processSpliceCompleteResponse(const CSpliceCompleteResponseMessage& msg)
{
  if (msg.getResult() != CBaseMessage::MR_SuccessfulResponse)
  {
    m_channelLogger->log(CLogLine::Warning, QString("Received SpliceResponse with result: %1").arg(CBaseMessage::toString(msg.getResult())));
    return false;
  }
  CSpliceSession* spliceSession = m_spliceSessions->getSpliceSession(msg.m_sessionId);
  if (!spliceSession)
  {
    m_channelLogger->log(CLogLine::Error, QString("Could not find SpliceSession with ID ").arg(msg.m_sessionId));
    return false;
  }
  if (msg.m_spliceTypeFlag == CSpliceCompleteResponseMessage::SpliceIn)
  {
    spliceSession->setNewState(CSpliceSession::SplicedIn);
    changeState(CAdState::StateOnInsertion);
  }
  else
  {
    spliceSession->setNewState(CSpliceSession::Finished);
    spliceSession->setDuration(msg.m_playedDuration);
  }
  return true;
}



void CStateOnMain::leaveState()
{
  CAdState::leaveState();
  // m_channelConfig->stopTimers();
  m_aliveTimer.stop();
}


void CStateOnMain::startAd(const CAdConfig& adConfig, const QDateTime& startTime)
{
  if (!m_stateIsActive)
    return;
  CSpliceRequestMessage msg;
  CSpliceSession* spliceSession = m_spliceSessions->createSpliceSession(startTime);
  spliceSession->setExpectedDuration(adConfig.m_duration);
  spliceSession->setNewState(CSpliceSession::Initializing);
  msg.m_sessionId = spliceSession->getSessionId();
  msg.m_priorSession = 0xFFFFFFFF;
  CTime time;
  time.fromTime(startTime);
  msg.m_time = time;
  msg.m_serviceId = adConfig.m_serviceId;
  msg.m_pcrPid = adConfig.m_pcrPid;
  msg.m_spliceElementaryStreams = adConfig.m_spliceElementaryStreams;
  msg.m_duration = adConfig.m_duration;
  msg.m_spliceEventId = 0xFFFFFFFF;
  msg.m_postBlack = adConfig.m_postBlack;
  msg.m_accessType = adConfig.m_accessType;
  msg.m_overridePlaying = adConfig.m_overridePlaying;
  msg.m_returnToPriorChannel = adConfig.m_overridePlaying;
  vector<uint8_t> data(4,0);  // bitrate rule and limit
  msg.m_spliceApiDescriptors.push_back(CSpliceApiDescriptor(0x1, data));
  sendMessage(&msg);
}




/**************************************************************************
 * CStateOnAd
 **************************************************************************/


CStateOnInsertion::CStateOnInsertion(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent):
    CAdState(channelConfig, spliceSessions, channelLogger, parent)
{
  m_aliveTimer.setInterval(60000);
  m_aliveTimer.setSingleShot(false);
  connect(&m_aliveTimer, SIGNAL(timeout()), this, SLOT(sendAliveMessage()));
}

/** Process a message
 *
 * @param Message
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CStateOnInsertion::processMessage(CBaseMessage* message)
{
  switch (message->getMessageId())
  {
  case CBaseMessage::MID_CueRequest:
    {
      CCueRequestMessage* msg = dynamic_cast<CCueRequestMessage*>(message);
      Q_ASSERT(msg);
      return processCueRequest(*msg);
    }
  case CBaseMessage::MID_GeneralResponse:
    return true;
  case CBaseMessage::MID_AliveResponse:
    {
      CAliveResponseMessage* msg = dynamic_cast<CAliveResponseMessage*>(message);
      Q_ASSERT(msg);
      return processAliveResponse(*msg);
    }
  case CBaseMessage::MID_SpliceResponse:
    {
      CSpliceResponseMessage* msg = dynamic_cast<CSpliceResponseMessage*>(message);
      Q_ASSERT(msg);
      return processSpliceResponse(*msg);
    }
  case CBaseMessage::MID_SpliceCompleteResponse:
    {
      CSpliceCompleteResponseMessage* msg = dynamic_cast<CSpliceCompleteResponseMessage*>(message);
      Q_ASSERT(msg);
      return processSpliceCompleteResponse(*msg);
    }
  default:
    m_channelLogger->log(CLogLine::Error, QString("received unexpected message %1").arg(message->getMessageId()));
    return false;
  }
}

bool CStateOnInsertion::processCueRequest(const CCueRequestMessage& cueRequest)
{
  CCueResponseMessage cueResponse;
  sendMessage(&cueResponse);
  if (cueRequest.m_spliceInfoSection.m_spliceCommandType == CSpliceInfoSection::SpliceInsert)
  {
    if (m_channelConfig->m_processScte35 &&
        cueRequest.m_spliceInfoSection.m_spliceInsert.m_outOfNetworkIndicator == 0)
    {
      qWarning("Splice out is not implemented!");
    }
  }
  else
  {
    m_channelLogger->log(CLogLine::Error, QString("Unknown splice command type: %1").arg(CSpliceInfoSection::toString(cueRequest.m_spliceInfoSection.m_spliceCommandType)));
    return false;
  }
  return true;
}

bool CStateOnInsertion::processAliveResponse(const CAliveResponseMessage& msg)
{
  if (msg.m_aliveState != CAliveResponseMessage::OnInsertion)
  {
    m_channelLogger->log(CLogLine::Error, "The channel is not on insertion! In another Ad Server running on this channel?");
  }
  return true;
}

bool CStateOnInsertion::processSpliceResponse(const CSpliceResponseMessage& msg)
{
if (msg.getResult() != CBaseMessage::MR_SuccessfulResponse)
{
  m_channelLogger->log(CLogLine::Warning, QString("Received SpliceResponse with result: %1").arg(CBaseMessage::toString(msg.getResult())));
  return false;
}
return true;
}

bool CStateOnInsertion::processSpliceCompleteResponse(const CSpliceCompleteResponseMessage& msg)
{
  if (msg.getResult() != CBaseMessage::MR_SuccessfulResponse)
  {
    m_channelLogger->log(CLogLine::Warning, QString("Received SpliceResponse with result: %1").arg(CBaseMessage::toString(msg.getResult())));
    return false;
  }
  CSpliceSession* spliceSession = m_spliceSessions->getSpliceSession(msg.m_sessionId);
  if (!spliceSession)
  {
    m_channelLogger->log(CLogLine::Error, QString("Could not find SpliceSession with ID ").arg(msg.m_sessionId));
    return false;
  }
  if (msg.m_spliceTypeFlag == CSpliceCompleteResponseMessage::SpliceIn)
  {
    spliceSession->setNewState(CSpliceSession::SplicedIn);
  }
  else
  {
    spliceSession->setNewState(CSpliceSession::Finished);
    spliceSession->setDuration(msg.m_playedDuration);
    changeState(CAdState::StateOnMain);
  }
  return true;
}

void CStateOnInsertion::enterState()
{
  CAdState::enterState();
  for (QList<CAdBlock>::iterator it = m_channelConfig->m_adBlocks.begin(); it != m_channelConfig->m_adBlocks.end(); ++it)
  {
    CAdBlock* adBlock = &(*it);
    connect(adBlock, SIGNAL(startB2bAd(CAdConfig,QDateTime)), this, SLOT(startB2bAd(CAdConfig,QDateTime)), Qt::UniqueConnection);
  }
  sendAliveMessage();
  m_aliveTimer.start();
}

void CStateOnInsertion::leaveState()
{
  m_aliveTimer.stop();
}


void CStateOnInsertion::startB2bAd(const CAdConfig& adConfig, const QDateTime& startTime)
{
  if (!m_stateIsActive)
    return;
  CSpliceRequestMessage msg;
  CSpliceSession* spliceSession = m_spliceSessions->createSpliceSession(startTime, m_spliceSessions->getLastSessionId());
  spliceSession->setExpectedDuration(adConfig.m_duration);
  spliceSession->setNewState(CSpliceSession::Initializing);
  msg.m_sessionId = spliceSession->getSessionId();
  msg.m_priorSession = spliceSession->getPriorSessionId();
  CTime time;
  time.fromTime(startTime);
  msg.m_time = time;
  msg.m_serviceId = adConfig.m_serviceId;
  msg.m_pcrPid = adConfig.m_pcrPid;
  msg.m_spliceElementaryStreams = adConfig.m_spliceElementaryStreams;
  msg.m_duration = adConfig.m_duration;
  msg.m_spliceEventId = 0xFFFFFFFF; // todo: fill in correct value for SCTE35
  msg.m_postBlack = adConfig.m_postBlack;
  msg.m_accessType = adConfig.m_accessType;
  msg.m_overridePlaying = adConfig.m_overridePlaying;
  msg.m_returnToPriorChannel = adConfig.m_overridePlaying;
  vector<uint8_t> data(4,0);  // bitrate rule and limit
  msg.m_spliceApiDescriptors.push_back(CSpliceApiDescriptor(0x1, data));
  sendMessage(&msg);
}

void CStateOnInsertion::sendAliveMessage()
{
  CAliveRequestMessage aliveMessage;
  sendMessage(&aliveMessage);
}
