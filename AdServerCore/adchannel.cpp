
#include <QTcpSocket>
#include <QBuffer>
#include <QDebug>
//#include <QMetaEnum>
#include <QFile>
#include <QXmlStreamWriter>

#include "adchannel.h"
#include "splicestates.h"
#include "scte30messages.h"
#include "splicesessions.h"

static quint16 s_Scte30Port = 6021;

/** Create an Ad channel
 *
 * @param parent
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CAdChannel::CAdChannel(QObject *parent) :
QObject(parent), m_socket(0), m_currentState(CAdState::StateNone)
{
  m_channelLogger = new CChannelLogger(this);
  m_spliceSessions = new CSpliceSessions(m_channelLogger, this);
  m_spliceStates[CAdState::StateNone] = new CStateNone(&m_channelConfig, m_spliceSessions, m_channelLogger, this);
  m_spliceStates[CAdState::StateInit] = new CStateInit(&m_channelConfig, m_spliceSessions, m_channelLogger, this);
  m_spliceStates[CAdState::StateOnMain] = new CStateOnMain(&m_channelConfig, m_spliceSessions, m_channelLogger, this);
  m_spliceStates[CAdState::StateOnInsertion] = new CStateOnInsertion(&m_channelConfig, m_spliceSessions, m_channelLogger, this);
  for (SpliceStatesMap_t::iterator it = m_spliceStates.begin(); it != m_spliceStates.end(); ++it)
  {
    CAdState* spliceState = it.value();
    connect(spliceState, SIGNAL(changeState(CAdState::AdState_t)), this, SLOT(changeState(CAdState::AdState_t)));
    connect(spliceState, SIGNAL(sendMessage(CBaseMessage*)), this, SLOT(sendMessage(CBaseMessage*)));
  }
}

CAdChannel::~CAdChannel()
{
  qWarning() << "deleting AdChannel";
  for (SpliceStatesMap_t::iterator it = m_spliceStates.begin(); it != m_spliceStates.end(); ++it)
  {
    delete it.value();
  }
  delete m_spliceSessions;
  delete m_channelLogger;

}


/** Set the splicer address
 *
 * @param SplicerAddress
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CAdChannel::setChannelConfig(const CChannelConfig& channelConfig)
{
  m_channelConfig = channelConfig;
}

/** Change the state of the state machine
 *
 * @param newState
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CAdChannel::changeState(CAdState::AdState_t newState)
{
  if (m_currentState == newState)
    return;
  m_channelLogger->log(CLogLine::Info, QString("Change state to %1").arg(CAdState::toString(newState)));
  m_spliceStates[m_currentState]->leaveState();
  m_currentState = newState;
  if (m_currentState == CAdState::StateNone)
  {
      if (m_socket)
      {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState && !m_socket->waitForDisconnected(3000))
          m_channelLogger->log(CLogLine::Warning, QString("Failed to disconnect because %1").arg(m_socket->errorString()));
      }
      m_unprocessedData.clear();
  }
  m_spliceStates[m_currentState]->enterState();
  stateChanged(newState);
}

/** Connect to the splicer
 *
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CAdChannel::connectSplicer()
{
  if (!m_socket)
  {
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
  }
  if (m_socket->state() != QAbstractSocket::UnconnectedState)
  {
    m_channelLogger->log(CLogLine::Error, "The TCP socket is already connected!");
    return false;
  }
  if (m_channelConfig.getSplicerAddress().isEmpty())
  {
    m_channelLogger->log(CLogLine::Error, "There is no splicer address defined");
    return false;
  }
  if (m_currentState != CAdState::StateNone)
  {
    m_channelLogger->log(CLogLine::Warning, QString("Trying to connect, but we are in a wrong splice state (%1)").arg(CAdState::toString(m_currentState)));
    changeState(CAdState::StateNone);
  }
  m_channelLogger->log(CLogLine::Info, QString("connecting to %1").arg(m_channelConfig.getSplicerAddress()));
  m_socket->connectToHost(m_channelConfig.getSplicerAddress(), s_Scte30Port);
  if (!m_socket->waitForConnected(3000))
  {
    m_channelLogger->log(CLogLine::Error, QString("Failed to connect to splicer on address %1 because %2").arg(m_channelConfig.getSplicerAddress(), m_socket->errorString()));
    return false;
  }
  changeState(CAdState::StateInit);
  return true;
}

/** Disconnect from the splicer
 *
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CAdChannel::disconnectSplicer()
{
  changeState(CAdState::StateNone);
  return true;
}

void CAdChannel::socketDisconnected()
{
  if (m_socket)
  {
    m_channelLogger->log(CLogLine::Info, QString("Connection closed because %1").arg(m_socket->errorString()));
  }
  else
  {
    m_channelLogger->log(CLogLine::Info, QString("Connection closed"));
  }
  changeState(CAdState::StateNone);
}


/** read new data from the socket
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CAdChannel::readData()
{
  if (m_socket->state() != QAbstractSocket::ConnectedState)
  {
    return;
  }
  if (m_unprocessedData.size() > 0)
  {
    QByteArray data = m_socket->readAll();
    m_unprocessedData.append(data);
    QBuffer buffer(&m_unprocessedData);
    buffer.open(QIODevice::ReadOnly);
    while (fullMessageAvailable(buffer))
    {
      parseMessage(buffer);
    }
  }
  else
  {
    while (m_socket && fullMessageAvailable(*m_socket))
    {
      parseMessage(*m_socket);
    }
    if (m_socket && m_socket->bytesAvailable() > 0)
    {
      QByteArray data = m_socket->readAll();
      m_unprocessedData.append(data);
    }
  }
}

/** Check if a full message is available in the buffer
 *
 * @param ioDevice
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
bool CAdChannel::fullMessageAvailable(QIODevice& ioDevice) const
{
  if (ioDevice.bytesAvailable() < 4)
    return false;
  QByteArray startData = ioDevice.peek(4);
  if (startData.size() < 4)
    return false;
  QDataStream stream(startData);
  quint16 messageId, messageSize;
  stream >> messageId;
  stream >> messageSize;
  return(m_socket->bytesAvailable() >= (messageSize + 8));
}

/** Parse a message from the buffer
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CAdChannel::parseMessage(QIODevice& ioDevice) const
{
  CBaseMessage baseMessage;
  baseMessage.read(ioDevice);
  if (baseMessage.getResult() != CBaseMessage::MR_SuccessfulResponse
      && baseMessage.getResult() != CBaseMessage::MR_NA)
  {
    m_channelLogger->log(CLogLine::Warning, QString("Response: %1").arg(CBaseMessage::toString(baseMessage.getResult())));
  }
  switch (baseMessage.getMessageId())
  {
  case CBaseMessage::MID_InitResponse:
    {
      CInitResponseMessage msg(baseMessage);
      msg.read(ioDevice);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
  case CBaseMessage::MID_CueRequest:
    {
      CCueRequestMessage msg(baseMessage);
      msg.read(ioDevice);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
  case CBaseMessage::MID_AliveResponse:
    {
      CAliveResponseMessage msg(baseMessage);
      msg.read(ioDevice);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
  case CBaseMessage::MID_SpliceResponse:
    {
      CSpliceResponseMessage msg(baseMessage);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
  case CBaseMessage::MID_SpliceCompleteResponse:
    {
      CSpliceCompleteResponseMessage msg(baseMessage);
      msg.read(ioDevice);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
  case CBaseMessage::MID_GeneralResponse:
    {
      CGeneralMessage msg (baseMessage);
      m_channelLogger->log(CLogLine::MessageRcv, msg.toString());
      m_spliceStates[m_currentState]->processMessage(&msg);
      break;
    }
    default:
    m_channelLogger->log(CLogLine::Warning, QString("Message of unknown type %1").arg(baseMessage.getMessageId()));
  }

}


void CAdChannel::sendMessage(CBaseMessage* msg)
{
  if (!m_socket)
  {
    m_channelLogger->log(CLogLine::Error, "The socket is not open!");
    return;
  }
  m_channelLogger->log(CLogLine::MessageSend, msg->toString());
  msg->write(*m_socket);
}

void CAdChannel::saveLog(const QString& fileName) const
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
    qWarning() << "Cannot write file " << file.errorString();
    return;
  }
  QXmlStreamWriter writer(&file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeDTD("<!DOCTYPE channelLog>");
  writer.writeStartElement("channelLog");
  writer.writeAttribute("version", "1.0");
  m_channelConfig.toXml(writer);
  m_spliceSessions->toXml(writer);
  m_channelLogger->toXml(writer);
  writer.writeEndElement();
  writer.writeEndDocument();
}

bool CAdChannel::loadLog(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    qWarning() << "Cannot read file %1" << file.errorString();
    return false;
  }
  QXmlStreamReader reader(&file);
  reader.readNextStartElement();
  if (reader.name() != "channelLog")
  {
    qWarning() <<  QString("Invalid file. No channelLog element found");
    return false;
  }
  if (reader.attributes().value("version") != "1.0")
  {
    qWarning() << QString("Wrong version");
    return false;
  }
  reader.readNext();

  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "channelLog"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "channelConfig")
      {
        if (!m_channelConfig.fromXml(reader))
          return false;
      }
      else if (reader.name() == "spliceSessions")
      {
        if (!m_spliceSessions->fromXml(reader))
          return false;
      }
      else if (reader.name() == "channelLogger")
      {
        if (!m_channelLogger->fromXml(reader))
          return false;
      }
      else
      {
        qWarning() << "ChannelLog: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}

