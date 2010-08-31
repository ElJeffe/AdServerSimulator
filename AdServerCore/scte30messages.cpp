#include "scte30messages.h"
#include <QIODevice>
#include <QDebug>
#include <QMetaEnum>
#include <QBuffer>
#include <QXmlStreamWriter>
#include "helpers.h"



/**************************************************************************
 * CBaseMessage
 **************************************************************************/

/** A base message
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CBaseMessage::CBaseMessage():
    QObject(0),
    m_messageId(MID_GeneralResponse),
    m_messageSize(0),
    m_result(MR_SuccessfulResponse),
    m_resultExtension(0)
{
}

/** copy constructor
 *
 * @param other
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CBaseMessage::CBaseMessage(const CBaseMessage& other):
    QObject(other.parent()),
    m_messageId(other.m_messageId),
    m_messageSize(other.m_messageSize),
    m_result(other.m_result),
    m_resultExtension(other.m_resultExtension)
{
}


/** A base message
 *
 * @param messageId
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CBaseMessage::CBaseMessage(MessageId_t messageId):
    m_messageId(messageId),
    m_messageSize(0),
    m_result(MR_SuccessfulResponse),
    m_resultExtension(0)
{
}

/** write a base message to a device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CBaseMessage::write(QIODevice& ioDevice) const
{
  quint16 messageSize = getDataSize();
  QDataStream(&ioDevice) << (quint16)m_messageId << messageSize << (quint16)m_result << m_resultExtension;
}

/** read a base message from a device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CBaseMessage::read(QIODevice& ioDevice)
{
  quint16 messageId,result;
  QDataStream stream(&ioDevice);

  stream >> messageId;
  m_messageId = (MessageId_t)messageId;
  stream >> m_messageSize;
  stream >> result;
  m_result = (MessageResult_t)result;
  stream >> m_resultExtension;
}

/** Convert to string
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QString CBaseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CBaseMessage {";
  stream << " messageId: " << m_messageId << ";";
  stream << " messageSize: " << m_messageSize << ";";
  stream << " result: " << m_result << ";";
  stream << " resultExtension: " << m_resultExtension << ";";
  stream << " }";
  return string;
}


QString CBaseMessage::toString(MessageId_t v)
{
  int enumIndex = CBaseMessage::staticMetaObject.indexOfEnumerator("MessageId_t");
  QMetaEnum metaEnum = CBaseMessage::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(v);
}

QString CBaseMessage::toString(MessageResult_t v)
{
  int enumIndex = CBaseMessage::staticMetaObject.indexOfEnumerator("MessageResult_t");
  QMetaEnum metaEnum = CBaseMessage::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(v);
}

/** pretty print a MessageId_t
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QTextStream& operator<<(QTextStream& dbg, CBaseMessage::MessageId_t v)
{
  dbg << CBaseMessage::toString(v);
  return dbg;
}

/** pretty print a MessageResult_t
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QTextStream& operator<<(QTextStream& dbg, CBaseMessage::MessageResult_t v)
{
  dbg << CBaseMessage::toString(v);
  return dbg;
}


/**************************************************************************
 * CHardwareConfig
 **************************************************************************/

/** define the hardware config
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CHardwareConfig::CHardwareConfig():
    m_length(8),
    m_chassis(0),
    m_card(0),
    m_port(0),
    m_logicalMultiplexType(LMT_NotUsed)
{

}

/** write the hardware config to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CHardwareConfig::write(QIODevice& ioDevice) const
{
  QDataStream(&ioDevice) << m_length << m_chassis << m_card << m_port << (quint16) m_logicalMultiplexType;
}
/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CHardwareConfig::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CHardwareConfig {";
  stream << " length: " << m_length << ";";
  stream << " chassis: " << m_chassis << ";";
  stream << " card: " << m_card << ";";
  stream << " port: " << m_port << ";";
  stream << " logicalMultiplexType: " << m_logicalMultiplexType << ";";
  stream << " }";
  return string;
}
/** pretty print a LogicalMultiplexType_t
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QTextStream& operator<<(QTextStream& dbg, CHardwareConfig::LogicalMultiplexType_t v)
{
  int enumIndex = CHardwareConfig::staticMetaObject.indexOfEnumerator("LogicalMultiplexType_t");
  QMetaEnum metaEnum = CHardwareConfig::staticMetaObject.enumerator(enumIndex);
  dbg << metaEnum.valueToKey(v);
  return dbg;
}

/**************************************************************************
 * CInitRequestData
 **************************************************************************/

/** Define an init request
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CInitRequestMessage::CInitRequestMessage():
  CBaseMessage(MID_InitRequest),
  m_version(1),
  m_channelName(32, 0),
  m_splicerName(32, 0)
{
  vector<quint8> data;
  data.push_back(3);
  m_spliceApiDescriptors.push_back(CSpliceApiDescriptor(3, data));
}

/** Set the channel name
 *
 * @param channelName
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CInitRequestMessage::setChannelName(QString channelName)
{
  m_channelName.fill(0);
  QTextStream(&m_channelName) << channelName.left(31);
}

/** set the splicer name
 *
 * @param splicerName
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CInitRequestMessage::setSplicerName(QString splicerName)
{
  m_splicerName.fill(0);
  QTextStream(&m_splicerName) << splicerName.left(31);
}

/** Get the data size
 *
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
quint16 CInitRequestMessage::getDataSize() const
{
  quint16 size = sizeof(m_version) + m_channelName.size() + m_splicerName.size() + m_hardwareConfig.getDataSize();
  for (vector<CSpliceApiDescriptor>::const_iterator it = m_spliceApiDescriptors.begin();
    it != m_spliceApiDescriptors.end(); ++it)
  {
    size += it->getSize();
  }
  return size;
}

/** Wrute the init message to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CInitRequestMessage::write(QIODevice& ioDevice) const
{

  CBaseMessage::write(ioDevice);
  QDataStream(&ioDevice) << m_version;
  ioDevice.write(m_channelName);
  ioDevice.write(m_splicerName);
  m_hardwareConfig.write(ioDevice);
  for (vector<CSpliceApiDescriptor>::const_iterator it = m_spliceApiDescriptors.begin();
    it != m_spliceApiDescriptors.end(); ++it)
  {
    it->write(ioDevice);
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CInitRequestMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CInitRequestMessage {";
  stream << CBaseMessage::toString();
  stream << " m_version: " << m_version << ";";
  stream << "  m_channelName: " << QString(m_channelName) << ";";
  stream << "  m_splicerName: " << QString(m_splicerName) << ";";
  stream << "  }";
  return string;
}


/**************************************************************************
 * CSpliceApiDescriptor
 **************************************************************************/

/** A splice API descriptor
 *
 * @param tag
 * @param data
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CSpliceApiDescriptor::CSpliceApiDescriptor(quint8 tag, vector<quint8> data):
    m_spliceDescriptorTag(tag),
    m_spliceApiIdentifier(0x53415049),
    m_data(data)
{
  m_descriptorLength = sizeof(m_spliceApiIdentifier) + m_data.size() * sizeof(quint8);
}

/** Get the data size of the descriptor
 *
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
quint16 CSpliceApiDescriptor::getSize() const
{
  return sizeof(m_spliceDescriptorTag) + sizeof(m_descriptorLength) + m_descriptorLength;
}

/** write an API descriptor to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceApiDescriptor::write(QIODevice& ioDevice) const
{
  QDataStream dataStream(&ioDevice);
  dataStream << m_spliceDescriptorTag;
  dataStream << m_descriptorLength;
  dataStream << m_spliceApiIdentifier;
  for (vector<quint8>::const_iterator it = m_data.begin(); it !=  m_data.end(); ++it)
  {
    dataStream << *it;
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceApiDescriptor::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceApiDescriptor {";
  stream << " spliceDescriptorTag: " << m_spliceDescriptorTag << ";";
  stream << " descriptorLength: " << m_descriptorLength << ";";
  stream << " spliceApiIdentifier: " << m_spliceApiIdentifier << ";";
  stream << " data {";
  for (vector<quint8>::const_iterator it = m_data.begin(); it !=  m_data.end(); ++it)
  {
    stream <<  QString("0x%1 ").arg(*it, 2, 16, QChar('0'));
  }
  stream << " }}";
  return string;
}



/**************************************************************************
 * CInitResponseData
 **************************************************************************/

/** An init response message
 *
 * @param baseMessage
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CInitResponseMessage::CInitResponseMessage(const CBaseMessage& baseMessage) :
    CBaseMessage(baseMessage),
    m_version(0),
    m_channelName(32, 0)
{
}

/** Read an init response message from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CInitResponseMessage::read(QIODevice& ioDevice)
{
  QDataStream stream(&ioDevice);

  stream >> m_version;
  m_channelName = ioDevice.read(32);
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CInitResponseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CInitResponseMessage {";
  stream << CBaseMessage::toString();
  stream << " version: " << m_version << ";";
  stream << " channelName: " << QString(m_channelName) << ";";
  stream << " }";
  return string;
}

/**************************************************************************
 * CTime
 **************************************************************************/

CTime::CTime():
    m_seconds(0),
    m_microseconds(0)
{
}

/** Read an time from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CTime::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);
  dataStream >> m_seconds;
  dataStream >> m_microseconds;
}

/** Write to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CTime::write(QIODevice& ioDevice) const
{
  QDataStream dataStream(&ioDevice);
  dataStream << m_seconds;
  dataStream << m_microseconds;
}


/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CTime::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CTime " << toDateTime().toString() << "{";
  stream << " seconds: " << m_seconds << ";";
  stream << " microseconds: " << m_microseconds << ";";
  stream << " }";
  return string;
}


void CTime::fromTime(const QDateTime& dateTime)
{
  m_seconds = dateTime.toTime_t();
  m_microseconds = dateTime.time().msec() * 1000;
}

QDateTime CTime::toDateTime() const
{
  QDateTime time;
  time.setTime_t(m_seconds);
  time.addMSecs(m_microseconds / 1000);
  return time;
}

quint16 CTime::getSize() const
{
  return sizeof(m_seconds) + sizeof(m_microseconds);
}


/**************************************************************************
 * CSpliceTime
 **************************************************************************/

/** create a splice time
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CSpliceTime::CSpliceTime():
  m_timeSpecifiedFlag(false),
  m_ptsTime(0)
{
}

/** create a splice time
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CSpliceTime::CSpliceTime(bool timeSpecifiedFlag, uint64_t ptsTime):
  m_timeSpecifiedFlag(timeSpecifiedFlag),
  m_ptsTime(ptsTime)
{
}


/** Read an splice time from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceTime::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);
  quint8 first;
  dataStream >> first;
  m_timeSpecifiedFlag = first & (1 << 7);
  if (m_timeSpecifiedFlag)
  {
    quint32 time;
    dataStream >> time;
    m_ptsTime = time;
    if (first & 1)
    {
      m_ptsTime &= ((uint64_t)1 << 32);
    }
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceTime::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceTime {";
  stream << " timeSpecifiedFlag: " << m_timeSpecifiedFlag << ";";
  if (m_timeSpecifiedFlag)
  {
    stream << " ptsTime: " << m_ptsTime << ";";
  }
  stream << " }";
  return string;
}


/**************************************************************************
 * CCueRequestMessage
 **************************************************************************/

/** Create a CueRequestTime
 *
 * @param baseMessage
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CCueRequestMessage::CCueRequestMessage(const CBaseMessage& baseMessage):
  CBaseMessage(baseMessage)
{
}

/** Read an Cue Request message from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CCueRequestMessage::read(QIODevice& ioDevice)
{
  m_time.read(ioDevice);
  m_spliceInfoSection.read(ioDevice);
}

/** Pretty print
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QString CCueRequestMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CCueRequestMessage {";
  stream << CBaseMessage::toString();
  stream << " time: " << m_time.toString() << ";";
  stream << " spliceInfoSection: " << m_spliceInfoSection.toString() << ";";
  stream << " }";
  return string;
}

/**************************************************************************
 * CCueResponseMessage
 **************************************************************************/

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CCueResponseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CCueResponseMessage {";
  stream << CBaseMessage::toString();
  stream << " }";
  return string;
}


/**************************************************************************
 * CSpliceSchedule
 **************************************************************************/

/** Read an CSpliceSchedule from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceSchedule::read(QIODevice& ioDevice)
{
  (void)ioDevice;
  Q_ASSERT(false);
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceSchedule::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " !!!! CSpliceSchedule is not yet implemented";
  return string;
}


/**************************************************************************
 * CSpliceInsert
 **************************************************************************/

/** Read an CSpliceInsert from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceInsert::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);
  quint8 u8;
  dataStream >> m_spliceEventId;
  dataStream >> u8;
  m_spliceEventCancelIndicator = u8 & (1 << 7);
  if (!m_spliceEventCancelIndicator)
  {
    dataStream >> u8;
    m_outOfNetworkIndicator = u8 & (1 << 7);
    m_programSpliceFlag = u8 & (1 << 6);
    m_durationFlag = u8 & (1 << 5);
    m_spliceImmediateFlag = u8 & (1 << 4);
    if (m_programSpliceFlag && !m_spliceImmediateFlag)
    {
      m_spliceTime.read(ioDevice);
    }
    if (!m_programSpliceFlag)
    {
      dataStream >> m_componentCount;
      for (quint8 i = 0; i < m_componentCount; ++i)
      {
        CComponent component;
        component.read(ioDevice, !m_spliceImmediateFlag);
        m_components.push_back(component);
      }
    }
    if (m_durationFlag)
    {
      m_breakDuration.read(ioDevice);
    }
    dataStream >> m_uniqueProgramId;
    dataStream >> m_availNum;
    dataStream >> m_availsExpected;
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceInsert::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceInsert {";
  stream << " spliceEventId: " << m_spliceEventId << ";";
  stream << " spliceEventCancelIndicator: " << m_spliceEventCancelIndicator << ";";
  if (!m_spliceEventCancelIndicator)
  {
    stream << " outOfNetworkIndicator: " << m_outOfNetworkIndicator << ";";
    stream << " programSpliceFlag: " << m_programSpliceFlag << ";";
    stream << " durationFlag: " << m_durationFlag << ";";
    stream << " spliceImmediateFlag: " << m_spliceImmediateFlag << ";";
    if (m_programSpliceFlag && !m_spliceImmediateFlag)
    {
      stream << " spliceTime: " << m_spliceTime.toString() << ";";
    }
    if (!m_programSpliceFlag)
    {
      stream << " componentCount: " << m_componentCount << ";";
      int i = 1;
      for (vector<CComponent>::const_iterator it = m_components.begin();
           it != m_components.end(); ++it)
      {
        stream << " component: " << i++ << " " << (*it).toString() << ";";
      }
    }
    if (m_durationFlag)
    {
      stream << " breakDuration: " << m_breakDuration.toString() << ";";
    }
    stream << " uniqueProgramId: " << m_uniqueProgramId << ";";
    stream << " availNum: " << m_availNum << ";";
    stream << " availsExpected: " << m_availsExpected << ";";
  }
  stream << " }";

  return string;
}

/**************************************************************************
 * CSpliceDescriptor
 **************************************************************************/

/** Read an CSpliceDescriptor from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceDescriptor::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);

  dataStream >> m_spliceDescriptorTag;
  dataStream >> m_descriptorLength;
  // read the rest of the packet in a buffer, s owe are sure that all data is read from the device
  QByteArray data = ioDevice.read(m_descriptorLength);
  QBuffer buffer(&data);
  buffer.open(QIODevice::ReadOnly);
  dataStream.setDevice(&buffer);

  dataStream >> m_identifier;
  while (buffer.bytesAvailable())
  {
    quint8 u8;
    dataStream >> u8;
    m_data.push_back(u8);
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceDescriptor::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceDescriptor {";
  stream << " spliceDescriptorTag: " << m_spliceDescriptorTag << ";";
  stream << " descriptorLength: " << m_descriptorLength << ";";
  stream << " identifier: " << m_identifier << ";";
  stream << " dataSize: " << m_data.size() << ";";
  return string;
}


/**************************************************************************
 * CSpliceInfoSection
 **************************************************************************/

/** Create a CSpliceInfoSection
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
CSpliceInfoSection::CSpliceInfoSection():
  m_eCrc(0)
{
}

/** Read an Splice Info Section from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceInfoSection::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);
  quint8 u8;
  quint16 u16;
  quint32 u32;
  dataStream >> m_tableId;
  dataStream >> u16;
  m_sectionSyntaxIndicator = u16 & (1 << 15);
  m_privateIndicator = u16 & (1 << 14);
  m_sectionLength = u16 & 0xFFF;
  // read the rest of the packet in a buffer, s owe are sure that all data is read from the device
  QByteArray data = ioDevice.read(m_sectionLength);
  QBuffer buffer(&data);
  buffer.open(QIODevice::ReadOnly);
  dataStream.setDevice(&buffer);

  dataStream >> m_protocolVersion;
  dataStream >> u8;
  m_encryptedPacket = u8 & (1 << 7);
  m_encryptionAlgorithm = (u8 >> 1) & 0x3F;
  dataStream >> u32;
  m_ptsAdjustment = u32;
  if (u8 & 1)
  {
    m_ptsAdjustment &= ((uint64_t)1 << 32);
  }
  dataStream >> m_cwIndex;
  dataStream >> u8;
  dataStream >> u16;
  m_spliceCommandLength = u16 & 0xFFF;
  dataStream >> u8;
  m_spliceCommandType = (SpliceCommandType_t) u8;
  switch (m_spliceCommandType)
  {
    case SpliceSchedule:
      m_spliceSchedule.read(buffer);
      break;
    case SpliceInsert:
      m_spliceInsert.read(buffer);
      break;
    case TimeSignal:
      m_timeSignal.read(buffer);
      break;
  default:
      break;
  }
  dataStream >> m_descriptorLoopLength;
  for (quint16 i = 0; i < m_descriptorLoopLength; ++i)
  {
    CSpliceDescriptor descriptor;
    descriptor.read(buffer);
    m_spliceDescriptors.push_back(descriptor);
  }
  // eat stuffing
  if (m_encryptedPacket)
  {
    while (buffer.bytesAvailable() > 4)
    {
      dataStream >> u8;
    }
    dataStream >> m_eCrc;
  }
  dataStream >> m_crc;

  if (buffer.bytesAvailable() != 0)
  {
    qWarning() << buffer.bytesAvailable() << " bytes left in buffer!";
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceInfoSection::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceInfoSection {";
  stream << " tableId: " << m_tableId << ";";
  stream << " sectionSyntaxIndicator: " << m_sectionSyntaxIndicator << ";";
  stream << " privateIndicator: " << m_privateIndicator << ";";
  stream << " sectionLength: " << m_sectionLength << ";";
  stream << " protocolVersion: " << m_protocolVersion << ";";
  stream << " encryptedPacket: " << m_encryptedPacket << ";";
  stream << " encryptionAlgorithm: " << m_encryptionAlgorithm << ";";
  stream << " cwIndex: " << m_cwIndex << ";";
  stream << " spliceCommandLength: " << m_spliceCommandLength << ";";
  stream << " spliceCommandType: " << m_spliceCommandType << ";";
  switch (m_spliceCommandType)
  {
  case CSpliceInfoSection::SpliceSchedule:
    stream << " spliceSchedule: " << m_spliceSchedule.toString() << ";";
      break;
  case CSpliceInfoSection::SpliceInsert:
      stream << " spliceInsert: " << m_spliceInsert.toString() << ";";
      break;
  case CSpliceInfoSection::TimeSignal:
      stream << " timeSignal: " << m_timeSignal.toString() << ";";
      break;
  default:
      break;
  }
  stream << " descriptorLoopLength: " << m_descriptorLoopLength << ";";
  int i = 0;
  for (vector<CSpliceDescriptor>::const_iterator it = m_spliceDescriptors.begin();
  it != m_spliceDescriptors.end(); ++it)
  {
    stream << " Descriptor " << i << ": " << (*it).toString() << ";";
  }
  if (m_encryptedPacket)
  {
    stream << " eCrc: " << m_eCrc << ";";
  }
  stream << " crc: " << m_crc << ";";
  stream << " }";
  return string;
}

QString CSpliceInfoSection::toString(SpliceCommandType_t v)
{
  int enumIndex = CSpliceInfoSection::staticMetaObject.indexOfEnumerator("SpliceCommandType_t");
  QMetaEnum metaEnum = CSpliceInfoSection::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(v);
}


/** Pretty print
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QTextStream& operator<<(QTextStream& dbg, CSpliceInfoSection::SpliceCommandType_t v)
{
  dbg << CSpliceInfoSection::toString(v);
  return dbg;
}

/**************************************************************************
 * CComponent
 **************************************************************************/

/** Read an Component from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CComponent::read(QIODevice& ioDevice, bool readTime)
{
  QDataStream dataStream(&ioDevice);

  dataStream >> m_componentTag;
  if (readTime)
  {
    m_spliceTime.read(ioDevice);
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CComponent::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CComponent {";
  stream << " componentTag: " << m_componentTag << ";";
  stream << " spliceTime: " << m_spliceTime.toString() << ";";
  stream << " }";
  return string;
}

/**************************************************************************
 * CBreakDuration
 **************************************************************************/

/** read a BreakDuration from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CBreakDuration::read(QIODevice& ioDevice)
{
  QDataStream dataStream(&ioDevice);
  quint8 u8;
  quint32 u32;
  dataStream >> u8;
  m_autoReturn = u8 & (1 << 7);
  dataStream >> u32;
  m_duration = u32;
  if (u8 & 1)
  {
    m_duration &= ((uint64_t)1 << 32);
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CBreakDuration::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CBreakDuration {";
  stream << " autoReturn: " << m_autoReturn << ";";
  stream << " duration: " << Helpers::ticksToString(m_duration) << ";";
  stream << " }";
  return string;
}

/**************************************************************************
 * CSpliceElementaryStream
 **************************************************************************/

/** Create a default ES
 *
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CSpliceElementaryStream::CSpliceElementaryStream():
    m_pid(0),
    m_streamType(0),
    m_avgBitrate(0xFFFFFFFF),
    m_maxBitrate(0xFFFFFFFF),
    m_minBitrate(0xFFFFFFFF),
    m_hResolution(0xFFFF),
    m_vResolution(0xFFFF)
{
}

/** Create an ES to splice
 *
 * @param pid The pid on the insertion stream to splice to
 * @param streamType Stream type of the ES to splice with
 * @param avgBitrate
 * @param maxBitrate
 * @param minBitrate
 * @param hResolution
 * @param vResolution
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CSpliceElementaryStream::CSpliceElementaryStream(quint16 pid, quint16 streamType, quint32 avgBitrate,
             quint32 maxBitrate, quint32 minBitrate, quint16 hResolution, quint16 vResolution):
  m_pid(pid),
  m_streamType(streamType),
  m_avgBitrate(avgBitrate),
  m_maxBitrate(maxBitrate),
  m_minBitrate(minBitrate),
  m_hResolution(hResolution),
  m_vResolution(vResolution)
{
}

/** Add a descriptor to the ES that will be used for matching (e.g. language)
 *
 * @param descriptor
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CSpliceElementaryStream::addDescriptor(const CDescriptor& descriptor)
{
  m_descriptors.push_back(descriptor);
}

/** Add a descriptor to the ES that will be used for matching (e.g. language)
 *
 * @param descriptor
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
void CSpliceElementaryStream::addDescriptor(quint8 tag, QList<quint8> data)
{
  addDescriptor(CDescriptor(tag, data));
}

/** Write to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceElementaryStream::write(QIODevice& ioDevice) const
{
  QDataStream dataStream(&ioDevice);
  dataStream << getDataSize();
  dataStream << m_pid;
  dataStream << m_streamType;
  dataStream << m_avgBitrate;
  dataStream << m_maxBitrate;
  dataStream << m_minBitrate;
  dataStream << m_hResolution;
  dataStream << m_vResolution;
  for (QList<CDescriptor>::const_iterator it = m_descriptors.begin(); it != m_descriptors.end(); ++it)
  {
    it->write(ioDevice);
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceElementaryStream::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceElementaryStream {";
  stream << " Size: " << getDataSize() << ";";
  stream << " pid: " << m_pid << ";";
  stream << " streamType: " << m_streamType << ";";
  stream << " avgBitrate: " << m_avgBitrate << ";";
  stream << " maxBitrate: " << m_maxBitrate << ";";
  stream << " minBitrate: " << m_minBitrate << ";";
  stream << " hResolution: " << m_hResolution << ";";
  stream << " vResolution: " << m_vResolution << ";";
  stream << " Descriptors: {";
  for (QList<CDescriptor>::const_iterator it = m_descriptors.begin(); it != m_descriptors.end(); ++it)
  {
    stream << (*it).toString() << ";";
  }
  stream << " }}";
  return string;

}

/** Get the size of this ES
 *
 *
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
quint8 CSpliceElementaryStream::getDataSize() const
{
  quint8 size = sizeof(quint8) /* length */ + sizeof(m_pid) + sizeof(m_streamType) + sizeof(m_avgBitrate) + sizeof(m_maxBitrate)
                + sizeof(m_minBitrate) + sizeof(m_hResolution) + sizeof(m_vResolution);
  for (QList<CDescriptor>::const_iterator it = m_descriptors.begin(); it != m_descriptors.end(); ++it)
  {
    size += it->getDataSize();
  }
  return size;
}

void CSpliceElementaryStream::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("spliceElementaryStream");

  Helpers::toXml(writer, "pid", m_pid);
  Helpers::toXml(writer, "streamType", m_streamType);
  Helpers::toXml(writer, "avgBitrate", m_avgBitrate);
  Helpers::toXml(writer, "maxBitrate", m_maxBitrate);
  Helpers::toXml(writer, "minBitrate", m_minBitrate);
  Helpers::toXml(writer, "hResolution", m_hResolution);
  Helpers::toXml(writer, "vResolution", m_vResolution);
  writer.writeStartElement("descriptors");
  for (QList<CDescriptor>::const_iterator it = m_descriptors.begin(); it != m_descriptors.end(); ++it)
  {
    it->toXml(writer);
  }
  writer.writeEndElement();
  writer.writeEndElement();
}

bool CSpliceElementaryStream::fromXml(QXmlStreamReader& reader)
{
  // check that it is really an SpliceElementaryStream
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "spliceElementaryStream")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "spliceElementaryStream"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "pid")
      {
        if (!Helpers::fromXml(reader, m_pid))
          return false;
      }
      else if (reader.name() == "streamType")
      {
        if (!Helpers::fromXml(reader, m_streamType))
          return false;
      }
      else if (reader.name() == "avgBitrate")
      {
        if (!Helpers::fromXml(reader, m_avgBitrate))
          return false;
      }
      else if (reader.name() == "maxBitrate")
      {
        if (!Helpers::fromXml(reader, m_maxBitrate))
          return false;
      }
      else if (reader.name() == "minBitrate")
      {
        if (!Helpers::fromXml(reader, m_minBitrate))
          return false;
      }
      else if (reader.name() == "hResolution")
      {
        if (!Helpers::fromXml(reader, m_hResolution))
          return false;
      }
      else if (reader.name() == "vResolution")
      {
        if (!Helpers::fromXml(reader, m_vResolution))
          return false;
      }
      else if (reader.name() == "streamType")
      {
        if (!Helpers::fromXml(reader, m_streamType))
          return false;
      }
      else if (reader.name() == "streamType")
      {
        if (!Helpers::fromXml(reader, m_streamType))
          return false;
      }
      else if (reader.name() == "descriptors")
      {
        reader.readNext();
        while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
                reader.name() == "descriptors"))
        {
          if(reader.tokenType() == QXmlStreamReader::StartElement)
          {

            if (reader.name() == "descriptor")
            {
              CDescriptor descriptor;
              if (!descriptor.fromXml(reader))
                return false;
              addDescriptor(descriptor);
            }
            else
            {
              qWarning() << "Decriptors: Unknown element in XML file: " << reader.name();
              return false;
            }
          }
          reader.readNext();
        }
      }
      else
      {
        qWarning() << "SpliceElementaryStream: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}


/**************************************************************************
 * CSpliceRequestMessage
 **************************************************************************/

/** Write to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CSpliceRequestMessage::write(QIODevice& ioDevice) const
{
  CBaseMessage::write(ioDevice);
  QDataStream dataStream(&ioDevice);
  dataStream << m_sessionId;
  dataStream << m_priorSession;
  m_time.write(ioDevice);
  dataStream << m_serviceId;
  if (m_serviceId == 0xFFFF)
  {
    dataStream << m_pcrPid;
    quint32 pidCount = m_spliceElementaryStreams.size();
    dataStream << pidCount;
    for (QList<CSpliceElementaryStream>::const_iterator it = m_spliceElementaryStreams.begin();
      it != m_spliceElementaryStreams.end(); ++it)
    {
      it->write(ioDevice);
    }
  }
  dataStream << m_duration;
  dataStream << m_spliceEventId;
  dataStream << m_postBlack;
  dataStream << m_accessType;
  dataStream << m_overridePlaying;
  dataStream << m_returnToPriorChannel;
  for (vector<CSpliceApiDescriptor>::const_iterator it = m_spliceApiDescriptors.begin();
    it != m_spliceApiDescriptors.end(); ++it)
  {
    it->write(ioDevice);
  }
}

quint16 CSpliceRequestMessage::getDataSize() const
{
  qint16 size = 0;
  size += sizeof(m_sessionId) + sizeof(m_priorSession) + m_time.getSize() + sizeof(m_serviceId);
  if (m_serviceId == 0xFFFF)
  {
    size += sizeof(m_pcrPid) + sizeof(quint32);
    for (QList<CSpliceElementaryStream>::const_iterator it = m_spliceElementaryStreams.begin();
      it != m_spliceElementaryStreams.end(); ++it)
    {
      size += it->getDataSize();
    }
  }
  size += sizeof(m_duration) + sizeof(m_spliceEventId) + sizeof(m_postBlack) + sizeof(m_accessType) + sizeof(m_overridePlaying) + sizeof(m_returnToPriorChannel);

  for (vector<CSpliceApiDescriptor>::const_iterator it = m_spliceApiDescriptors.begin();
    it != m_spliceApiDescriptors.end(); ++it)
  {
    size += it->getSize();
  }
  return size;
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceRequestMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceRequestMessage {";
  stream << CBaseMessage::toString();
  stream << " sessionId: " << m_sessionId << ";";
  stream << " priorSession: " << m_priorSession << ";";
  stream << " time: " << m_time.toString() << ";";
  stream << " serviceId: " << m_serviceId << ";";
  if (m_serviceId == 0xFFFF)
  {
    stream << " pcrPid: " << m_pcrPid << ";";
    stream << " SpliceElementaryStreams: {";
    for (QList<CSpliceElementaryStream>::const_iterator it = m_spliceElementaryStreams.begin();
      it != m_spliceElementaryStreams.end(); ++it)
    {
      stream << (*it).toString() << ";";
    }
    stream << " }";
  }
  stream << " duration: " << Helpers::ticksToString(m_duration) << ";";
  stream << " spliceEventId: " << m_spliceEventId << ";";
  stream << " postBlack: " << Helpers::ticksToString(m_postBlack) << ";";
  stream << " accessType: " << m_accessType << ";";
  stream << " overridePlaying: " << m_overridePlaying << ";";
  stream << " returnToPriorChannel: " << m_returnToPriorChannel << ";";
  stream << " SpliceApiDescriptors: {";
  for (vector<CSpliceApiDescriptor>::const_iterator it = m_spliceApiDescriptors.begin();
    it != m_spliceApiDescriptors.end(); ++it)
  {
    stream << (*it).toString() << ";";
  }
  stream << " }}";
  return string;
}

/**************************************************************************
 * CDescriptor
 **************************************************************************/

/** Create a descriptor
 *
 * @param tag The tag of the descriptor
 * @param data the data
 * @return
 *
 * History:
 * - 2010/08/05: STEELJ  - Initial Version.
  */
CDescriptor::CDescriptor(quint8 tag, QList<quint8> data):
    m_tag(tag),
    m_data(data)
{
}

CDescriptor::CDescriptor():
    m_tag(0)
{
}

/** Write to the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
void CDescriptor::write(QIODevice& ioDevice) const
{
  QDataStream dataStream(&ioDevice);
  dataStream << m_tag;
  dataStream << (quint8) m_data.size();
  for (QList<quint8>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
  {
    dataStream << *it;
  }
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CDescriptor::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CDescriptor {";
  stream << " tag: " << m_tag << ";";
  stream << " lenght: " << m_data.size() << ";";
  stream << " data {";
  for (QList<quint8>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
  {
    stream <<  QString("0x%1 ").arg(*it, 2, 16, QChar('0'));
  }
  stream << " }}";
  return string;
}

/** get the data size of the descriptor
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
quint16 CDescriptor::getDataSize() const
{
  return sizeof(m_tag) + sizeof(quint8) /*length*/ + m_data.size() * sizeof(quint8);
}

QString CDescriptor::dataToString() const
{
  QString dataString;
  for (QList<quint8>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
  {
    dataString.append(QString("%1").arg(*it, 2, 16, QChar('0')));
  }
  return dataString;
}
bool CDescriptor::dataFromString(const QString& string)
{
  int pos = 0;
  while (pos < string.size())
  {
    bool res;
    quint8 u8 = string.mid(pos, 2).toUShort(&res, 16);
    if (!res)
      return false;
    m_data.push_back(u8);
    pos += 2;
  }
  return true;
}


void CDescriptor::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("descriptor");

  Helpers::toXml(writer, "tag", m_tag);
  Helpers::toXml(writer, "data", dataToString());
  writer.writeEndElement();
}

bool CDescriptor::fromXml(QXmlStreamReader& reader)
{
  // check that it is really an SpliceElementaryStream
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "descriptor")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "descriptor"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "tag")
      {
        if (!Helpers::fromXml(reader, m_tag))
          return false;
      }
      else if (reader.name() == "data")
      {
        QString dataString;
        if (!Helpers::fromXml(reader, dataString))
          return false;
        dataFromString(dataString);
      }
      else
      {
        qWarning() << "Descriptor: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}



/**************************************************************************
 * CAliveRequestMessage
 **************************************************************************/

CAliveRequestMessage::CAliveRequestMessage():
  CBaseMessage(MID_AliveRequest)
{
  m_time.fromTime(QDateTime().currentDateTime());
}

void CAliveRequestMessage::write(QIODevice& ioDevice) const
{
  CBaseMessage::write(ioDevice);
  m_time.write(ioDevice);
}

quint16 CAliveRequestMessage::getDataSize() const
{
  return m_time.getSize();
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CAliveRequestMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CAliveRequestMessage {";
  stream << CBaseMessage::toString();
  stream << " time: " << m_time.toString() << ";";
  stream << " }";
  return string;
}

/**************************************************************************
 * CAliveResponseMessage
 **************************************************************************/

void CAliveResponseMessage::read(QIODevice& ioDevice)
{
  QDataStream stream(&ioDevice);

  quint32 u32;
  stream >> u32;
  m_aliveState = (AliveState_t) u32;
  stream >> m_sessionId;
  m_time.read(ioDevice);
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CAliveResponseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CAliveResponseMessage {";
  stream << CBaseMessage::toString();
  stream << " aliveState: " << m_aliveState << ";";
  stream << " sessionId: " << m_sessionId << ";";
  stream << " time: " << m_time.toString() << ";";
  stream << " }";
  return string;
}

/** Get the data size of the message
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
quint16 CAliveResponseMessage::getDataSize() const
{
  return sizeof(quint32) + sizeof(m_sessionId) + m_time.getSize();
}

QString CAliveResponseMessage::toString(AliveState_t v)
{
  int enumIndex = CAliveResponseMessage::staticMetaObject.indexOfEnumerator("AliveState_t");
  QMetaEnum metaEnum = CAliveResponseMessage::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(v);
}

/** pretty print a MessageId_t
 *
 * @param dbg
 * @param v
 *
 * @return
 *
 * History:
 * - 2010/08/03: STEELJ  - Initial Version.
  */
QTextStream &operator<<(QTextStream& dbg, CAliveResponseMessage::AliveState_t v)
{
  dbg << CAliveResponseMessage::toString(v);
  return dbg;
}


/**************************************************************************
 * CSpliceResponseMessage
 **************************************************************************/

/** create a SpliceResponseMessage
 *
 * @param baseMessage
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
CSpliceResponseMessage::CSpliceResponseMessage(const CBaseMessage& baseMessage):
    CBaseMessage(baseMessage)
{
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceResponseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceResponseMessage {";
  stream << CBaseMessage::toString();
  stream << " }";
  return string;
}

/**************************************************************************
 * CGeneralMessage
 **************************************************************************/

CGeneralMessage::CGeneralMessage(const CBaseMessage& baseMessage) :
    CBaseMessage(baseMessage)
{
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CGeneralMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CGeneralMessage {";
  stream << CBaseMessage::toString();
  stream << " }";
  return string;
}

/**************************************************************************
 * CSpliceCompleteResponseMessage
 **************************************************************************/

/** Create a SpliceCompleteResponse message
 *
 * @param baseMessage
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
CSpliceCompleteResponseMessage::CSpliceCompleteResponseMessage(const CBaseMessage& baseMessage):
    CBaseMessage(baseMessage)
{
}

/** read the message from the device
 *
 * @param ioDevice
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
void CSpliceCompleteResponseMessage::read(QIODevice& ioDevice)
{
  QDataStream stream(&ioDevice);
  quint8 u8;
  stream >> m_sessionId;
  stream >> u8;
  m_spliceTypeFlag = (SpliceTypeFlag_t)u8;
  stream >> m_bitrate;
  stream >> m_playedDuration;
}

/** convert to string
 *
 *
 * @return
 *
 * History:
 * - 2010/08/09: STEELJ  - Initial Version.
  */
QString CSpliceCompleteResponseMessage::toString() const
{
  QString string;
  QTextStream stream(&string);
  stream << " CSpliceCompleteResponseMessage {";
  stream << CBaseMessage::toString();
  stream << " sessionId: " << m_sessionId << ";";
  stream << " spliceTypeFlag: " << m_spliceTypeFlag << ";";
  stream << " bitrate: " << m_bitrate << ";";
  stream << " playedDuration: " << Helpers::ticksToString(m_playedDuration) << ";";
  stream << " }";
  return string;
}
