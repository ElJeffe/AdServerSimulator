#include "channelconfig.h"
#include <QDebug>
#include <QTimer>
#include <QXmlStreamWriter>
#include "helpers.h"
#define INVALID_B2B_INDEX 0xffffff
/**************************************************************************
 * CChannelConfig
 **************************************************************************/

/** Create a default ChannelConfig
 *
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CChannelConfig::CChannelConfig():
    m_splicerAddress("0.0.0.0"),
    m_channelName("new channel"),
    m_splicerName(),
    m_chassis(0),
    m_card(0),
    m_port(0),
    m_secondsBeforeSpliceRequest(3),
    m_processScte35(true)
{
}

/** Create a ChannelConfig
 *
 * @param splicerAddress The IP address of the splicer
 * @param channelName The name of the channel (as configured in DCM)
 * @param splicerName The name of the splicer
 * @param chassis The chassis number where the insertion stream can be found
 * @param card The chassis number where the insertion stream can be found
 * @param port The port number where the insertion stream can be found
 * @param secondsBeforeSpliceRequest Time before a splice request is sent
 * @param processScte35 Set to true if the AdServer should splice when SCTE35 messages are received
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CChannelConfig::CChannelConfig(const QString& splicerAddress, const QString& channelName, const QString& splicerName,
               int chassis, int card, int port, int secondsBeforeSpliceRequest, bool processScte35 ):
m_splicerAddress(splicerAddress),
m_channelName(channelName),
m_splicerName(splicerName),
m_chassis(chassis),
m_card(card),
m_port(port),
m_secondsBeforeSpliceRequest(secondsBeforeSpliceRequest),
m_processScte35(processScte35)
{
}

/** Add an AdBlock to the Channel
 *
 * @param adBlock
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CChannelConfig::addAdBlock(const CAdBlock& adBlock)
{
  m_adBlocks.push_back(adBlock);
}

void CChannelConfig::startTimers()
{
  for (QList<CAdBlock>::iterator it = m_adBlocks.begin();
    it != m_adBlocks.end(); ++it)
  {
    it->startTimer(m_secondsBeforeSpliceRequest);
  }
}

void CChannelConfig::stopTimers()
{
  for (QList<CAdBlock>::iterator it = m_adBlocks.begin();
    it != m_adBlocks.end(); ++it)
  {
    it->stopTimer();
  }
}


void CChannelConfig::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("channelConfig");
  Helpers::toXml(writer, "splicerAddress", m_splicerAddress);
  Helpers::toXml(writer, "channelName", m_channelName);
  Helpers::toXml(writer, "splicerName", m_splicerName);
  Helpers::toXml(writer, "chassis", m_chassis);
  Helpers::toXml(writer, "card", m_card);
  Helpers::toXml(writer, "port", m_port);
  Helpers::toXml(writer, "secondsBeforeSpliceRequest", m_secondsBeforeSpliceRequest);
  Helpers::toXml(writer, "processScte35", m_processScte35);
  writer.writeStartElement("adBlocks");
  for (QList<CAdBlock>::const_iterator it = m_adBlocks.begin();
    it != m_adBlocks.end(); ++it)
  {
    it->toXml(writer);
  }
  writer.writeEndElement();
  writer.writeEndElement();
}

bool CChannelConfig::fromXml(QXmlStreamReader& reader)
{
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "channelConfig")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "channelConfig"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "splicerAddress")
      {
        if (!Helpers::fromXml(reader, m_splicerAddress))
          return false;
      }
      else if (reader.name() == "channelName")
      {
        if (!Helpers::fromXml(reader, m_channelName))
          return false;
      }
      else if (reader.name() == "splicerName")
      {
        if (!Helpers::fromXml(reader, m_splicerName))
          return false;
      }
      else if (reader.name() == "chassis")
      {
        if (!Helpers::fromXml(reader, m_chassis))
          return false;
      }
      else if (reader.name() == "card")
      {
        if (!Helpers::fromXml(reader, m_card))
          return false;
      }
      else if (reader.name() == "port")
      {
        if (!Helpers::fromXml(reader, m_port))
          return false;
      }
      else if (reader.name() == "secondsBeforeSpliceRequest")
      {
        if (!Helpers::fromXml(reader, m_secondsBeforeSpliceRequest))
          return false;
      }
      else if (reader.name() == "processScte35")
      {
        if (!Helpers::fromXml(reader, m_processScte35))
          return false;
      }
      else if (reader.name() == "adBlocks")
      {
        reader.readNext();
        while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
                reader.name() == "adBlocks"))
        {
          if(reader.tokenType() == QXmlStreamReader::StartElement)
          {

            if (reader.name() == "adBlock")
            {
              CAdBlock adBlock;
              if (!adBlock.fromXml(reader))
                return false;
              addAdBlock(adBlock);
            }
            else
            {
              qWarning() << "AdBlocks: Unknown element in XML file: " << reader.name();
              return false;
            }
          }
          reader.readNext();
        }
      }
      else
      {
        qWarning() << "ChannelConfig: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}


/**************************************************************************
 * CAdBlock
 **************************************************************************/

/** Create an Ad Block
 *
 * @param startTime The first time when a splice should happen (set to 0 if splicing will happen with SCTE35 messages)
 * @param recurringTime The recurring period of the ad
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CAdBlock::CAdBlock(const QDateTime& startTime, const QTime& recurringTime, const QString& name):
  QObject(),
  m_name(name),
  m_startTime(startTime),
  m_recurringTime(recurringTime),
  m_adTimer(0),
  m_b2bAdTimer(0),
  m_nextAdTime(QDateTime()),
  m_nextB2bAdTime(QDateTime()),
  m_nextB2bIndex(INVALID_B2B_INDEX)
{
}


CAdBlock::CAdBlock(const CAdBlock& other):
    QObject(),
    m_name(other.m_name),
    m_startTime(other.m_startTime),
    m_recurringTime(other.m_recurringTime),
    m_adConfigs(other.m_adConfigs),
    m_adTimer(0),
    m_b2bAdTimer(0),
    m_nextAdTime(QDateTime()),
    m_nextB2bAdTime(QDateTime()),
    m_nextB2bIndex(INVALID_B2B_INDEX)
{
}

CAdBlock& CAdBlock::operator=(const CAdBlock& other)
{
  m_name = other.m_name;
  m_startTime = other.m_startTime;
  m_recurringTime = other.m_recurringTime;
  m_adConfigs = other.m_adConfigs;
  return *this;
}

CAdBlock::~CAdBlock()
{
  stopTimer();
}



/** Add an ad to the AdBlock
 *
 * @param adConfig
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CAdBlock::addAdConfig(const CAdConfig& adConfig)
{
  m_adConfigs.push_back(adConfig);
}

void CAdBlock::startTimer(int secondsBeforeSpliceRequest)
{
  if (m_adTimer)
  {
    return;
  }
  m_secondsBeforeSpliceRequest = secondsBeforeSpliceRequest;
  QDateTime now = QDateTime::currentDateTime().toUTC().addSecs(secondsBeforeSpliceRequest);
  m_nextAdTime = m_startTime.toUTC();
  quint32 recTimeInSec = m_recurringTime.hour() * 3600 + m_recurringTime.minute() * 60 + m_recurringTime.second();
  if (recTimeInSec > 0)
  {
    // convert to next time in future
    if (m_nextAdTime < now)
    {
      int secToNow = m_nextAdTime.secsTo(now);
      int timeToAdd = secToNow / recTimeInSec;
      m_nextAdTime = m_nextAdTime.addSecs((timeToAdd + 1) * recTimeInSec);
    }
  }
  if (m_nextAdTime >= now)
  {
    int msecToAd = QDateTime(now.date()).secsTo(QDateTime(m_nextAdTime.date())) * 1000;
    msecToAd += now.time().msecsTo(m_nextAdTime.time());
    if (msecToAd < 1000)
      msecToAd += recTimeInSec * 1000;
    m_adTimer = new QTimer();
    m_adTimer->setSingleShot(true);
    m_adTimer->start(msecToAd);
    connect(m_adTimer, SIGNAL(timeout()), this, SLOT(adTimerExpired()));
  }
}

void CAdBlock::stopTimer()
{
  if (m_adTimer)
  {
    m_adTimer->stop();
    delete m_adTimer;
    m_adTimer = 0;
  }
  if (m_b2bAdTimer)
  {
    m_b2bAdTimer->stop();
    delete m_b2bAdTimer;
    m_b2bAdTimer = 0;
  }
}

void CAdBlock::adTimerExpired()
{
  qDebug() << QTime().currentTime().toString("h m s zzz") << "ad timer expired";
  if (!m_adConfigs.empty())
  {
    CAdConfig firstAd = m_adConfigs.front();
    startAd(firstAd, m_nextAdTime);
    if (m_nextB2bIndex == INVALID_B2B_INDEX)
      startB2bTimer(m_nextAdTime.addMSecs(QTime().msecsTo(firstAd.getTotalDuration())));
  }
  // set next ad time
  m_nextAdTime = m_nextAdTime.addSecs(QTime().secsTo(m_recurringTime));
  // restart the timer if recurrent time is set
  if (m_recurringTime.isValid())
  {
    QDateTime now = QDateTime().currentDateTime().toUTC();
    int msecToAd = QDateTime(now.date()).secsTo(QDateTime(m_nextAdTime.date())) * 1000;
    msecToAd += now.time().msecsTo(m_nextAdTime.time());
    m_adTimer->start(msecToAd - (m_secondsBeforeSpliceRequest * 1000));
  }
}

void CAdBlock::setScte35StartTime(QDateTime firstAdStartTime)
{
  if (!m_adConfigs.empty())
  {
    CAdConfig firstAd = m_adConfigs.front();
    startB2bTimer(firstAdStartTime.addMSecs(QTime().msecsTo(firstAd.getTotalDuration())));
  }
}

void CAdBlock::startB2bTimer(QDateTime b2bAdTime)
{
  qWarning() << "Start B2B timer for time " << b2bAdTime.toString();
  if (m_nextB2bIndex == INVALID_B2B_INDEX)
    m_nextB2bIndex = 1;
  if (m_adConfigs.size() - 1 < m_nextB2bIndex)
  {
    // no more B2B ads present
    m_nextB2bIndex = INVALID_B2B_INDEX;
    return;
  }
  m_nextB2bAdTime = b2bAdTime;
  if (!m_b2bAdTimer)
  {
    m_b2bAdTimer = new QTimer();
    m_b2bAdTimer->setSingleShot(true);
    connect(m_b2bAdTimer, SIGNAL(timeout()), this, SLOT(b2bAdTimerExpired()));

  }
  m_b2bAdTimer->start((QDateTime().currentDateTime().secsTo(m_nextB2bAdTime) - m_secondsBeforeSpliceRequest) * 1000);
}

void CAdBlock::b2bAdTimerExpired()
{
  if (m_adConfigs.size() - 1 < m_nextB2bIndex)
  {
    // no more B2B ads present
    m_nextB2bIndex = INVALID_B2B_INDEX;
    return;
  }
  const CAdConfig& adConfig = m_adConfigs[m_nextB2bIndex];
  startB2bAd(adConfig, m_nextB2bAdTime);

  // start next B2B ad
  ++m_nextB2bIndex;
  if (m_adConfigs.size() - 1 < m_nextB2bIndex)
  {
    // no more B2B ads present
    m_nextB2bIndex = INVALID_B2B_INDEX;
    return;
  }
  startB2bTimer(m_nextB2bAdTime.addMSecs(QTime().msecsTo(adConfig.getTotalDuration())));
}

void CAdBlock::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("adBlock");
  Helpers::toXml(writer, "name", m_name);
  Helpers::toXml(writer, "startTime", m_startTime);
  Helpers::toXml(writer, "recurringTime", m_recurringTime);
  writer.writeStartElement("adConfigs");
  for (QList<CAdConfig>::const_iterator it = m_adConfigs.begin(); it != m_adConfigs.end(); ++it)
  {
    it->toXml(writer);
  }
  writer.writeEndElement();
  writer.writeEndElement();
}

bool CAdBlock::fromXml(QXmlStreamReader& reader)
{
  // check that it is really an adBlock
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "adBlock")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "adBlock"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "name")
      {
        if (!Helpers::fromXml(reader, m_name))
          return false;
      }
      else if (reader.name() == "startTime")
      {
        if (!Helpers::fromXml(reader, "startTime", m_startTime))
          return false;
      }
      else if (reader.name() == "recurringTime")
      {
        if (!Helpers::fromXml(reader, "recurringTime", m_recurringTime))
          return false;
      }
      else if (reader.name() == "adConfigs")
      {
        reader.readNext();
        while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
                reader.name() == "adConfigs"))
        {
          if(reader.tokenType() == QXmlStreamReader::StartElement)
          {

            if (reader.name() == "adConfig")
            {
              CAdConfig adConfig;
              if (!adConfig.fromXml(reader))
                return false;
              addAdConfig(adConfig);
            }
            else
            {
              qWarning() << "AdConfigs: Unknown element in XML file: " << reader.name();
              return false;
            }
          }
          reader.readNext();
        }
      }
      else
      {
        qWarning() << "AdBlock: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }

  return true;
}


/**************************************************************************
 * CAdConfig
 **************************************************************************/

/** Create a default ad
 *
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CAdConfig::CAdConfig():
    m_serviceId(0xFFFF),
    m_pcrPid(0),
    m_duration(0),
    m_postBlack(0xFFFFFFFF),
    m_accessType(0),
    m_overridePlaying(false),
    m_returnToPriorChannel(true)
{
}

/** Create an ad
 *
 * @param pcrPid The PCR pid of the insertion stream
 * @param serviceId The SID of the insertion service (set to 0xFFFF in case of component splicing)
 * @param durationInTicks Duration of the Ad in 90Hz ticks (use setDuration for human readable time)
 * @param postBlackInTicks Duration of black frames after splice is finished (use setPostBlack for human readable time)
 * @param accessType priority of the Ad
 * @param overridePlaying Set to true to override currently playing ads
 * @param returnToPriorChannel Return to the prior channel if the duration is over
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CAdConfig::CAdConfig(quint16 pcrPid, quint16 serviceId, quint32 durationInTicks,
          quint32 postBlackInTicks, quint8 accessType, bool overridePlaying,
          bool returnToPriorChannel):
m_serviceId(serviceId),
m_pcrPid(pcrPid),
m_duration(durationInTicks),
m_postBlack(postBlackInTicks),
m_accessType(accessType),
m_overridePlaying(overridePlaying),
m_returnToPriorChannel(returnToPriorChannel)
{
}

/** Add an ES splice
 *
 *  Set ServiceId of the AdConfig to 0xFFFF if this mode is used!
 *
 * @param spliceElementaryStream The ES to splice
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CAdConfig::addElementaryStream(const CSpliceElementaryStream& spliceElementaryStream)
{
  m_spliceElementaryStreams.push_back(spliceElementaryStream);
}

/** Set the duration of the ad in human readable format
 *
 * @param duration
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CAdConfig::setDuration(const QTime& duration)
{
  m_duration = Helpers::timeToTicks(duration);
}

QTime CAdConfig::getTotalDuration() const
{
  quint32 totalTicks = m_duration;
  if (m_postBlack != 0xffffffff)
    totalTicks += m_postBlack;
  return Helpers::ticksToTime(totalTicks);
}

void CAdConfig::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("adConfig");
  Helpers::toXml(writer, "serviceId", m_serviceId);
  Helpers::toXml(writer, "pcrPid", m_pcrPid);
  Helpers::toXml(writer, "duration", Helpers::ticksToTime(m_duration));
  Helpers::toXml(writer, "postBlack", m_postBlack);
  Helpers::toXml(writer, "accessType", m_accessType);
  Helpers::toXml(writer, "overridePlaying", m_overridePlaying);
  Helpers::toXml(writer, "returnToPriorChannel", m_returnToPriorChannel);

  writer.writeStartElement("spliceElementaryStreams");
  for (QList<CSpliceElementaryStream>::const_iterator it = m_spliceElementaryStreams.begin(); it != m_spliceElementaryStreams.end(); ++it)
  {
    it->toXml(writer);
  }
  writer.writeEndElement();
  writer.writeEndElement();
}

bool CAdConfig::fromXml(QXmlStreamReader& reader)
{
  // check that it is really an adConfig
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "adConfig")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "adConfig"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "serviceId")
      {
        if (!Helpers::fromXml(reader, m_serviceId))
          return false;
      }
      else if (reader.name() == "pcrPid")
      {
        if (!Helpers::fromXml(reader, m_pcrPid))
          return false;
      }
      else if (reader.name() == "duration")
      {
        QTime time;
        if (!Helpers::fromXml(reader, "duration", time))
          return false;
        m_duration = Helpers::timeToTicks(time);
      }
      else if (reader.name() == "postBlack")
      {
        if (!Helpers::fromXml(reader, m_postBlack))
          return false;
      }
      else if (reader.name() == "accessType")
      {
        if (!Helpers::fromXml(reader, m_accessType))
          return false;
      }
      else if (reader.name() == "overridePlaying")
      {
        if (!Helpers::fromXml(reader, m_overridePlaying))
          return false;
      }
      else if (reader.name() == "returnToPriorChannel")
      {
        if (!Helpers::fromXml(reader, m_returnToPriorChannel))
          return false;
      }
      else if (reader.name() == "spliceElementaryStreams")
      {
        reader.readNext();
        while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
                reader.name() == "spliceElementaryStreams"))
        {
          if(reader.tokenType() == QXmlStreamReader::StartElement)
          {

            if (reader.name() == "spliceElementaryStream")
            {
              CSpliceElementaryStream es;
              if (!es.fromXml(reader))
                return false;
              addElementaryStream(es);
            }
            else
            {
              qWarning() << "SpliceElementaryStreams: Unknown element in XML file: " << reader.name();
              return false;
            }
          }
          reader.readNext();
        }
      }
      else
      {
        qWarning() << "AdConfig: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }

  return true;
}


