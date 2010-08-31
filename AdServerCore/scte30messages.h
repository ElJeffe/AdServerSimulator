#ifndef SCTE30MESSAGES_H
#define SCTE30MESSAGES_H

#include <stdint.h>
#include <vector>
#include <QString>
#include <QObject>
#include <QDateTime>
#include <QTextStream>
#include <QXmlStreamWriter>

using namespace std;

/********************
 * Forward declarations
 ********************/
class CHardwareConfig;
class CSpliceApiDescriptor;
class QIODevice;
class QXmlStreamWriter;

/***********************
 * Helper classes
 ***********************/
class CBaseMessage: public QObject
{
  Q_OBJECT
  Q_ENUMS(MessageId_t)
  Q_ENUMS(MessageResult_t)
public:
  enum MessageId_t
  {
    MID_GeneralResponse = 0,
    MID_InitRequest,
    MID_InitResponse,
    MID_ExtendedDataRequest,
    MID_ExtendedDataResponse,
    MID_AliveRequest,
    MID_AliveResponse,
    MID_SpliceRequest,
    MID_SpliceResponse,
    MID_SpliceCompleteResponse,
    MID_GetConfigRequest,
    MID_GetConfigResponse,
    MID_CueRequest,
    MID_CueResponse,
    MID_AbortRequest,
    MID_AbortResponse,
    MID_TearDownFeedRequest,
    MID_TearDownFeedResponse
  };
  enum MessageResult_t
  {
    MR_SuccessfulResponse = 100,
    MR_UnknownFailure,
    MR_InvalidVersion,
    MR_AccessDenied,
    MR_InvalidUnknownChannelName,
    MR_InvalidPhysicalConnection,
    MR_NoConfigurationFound,
    MR_InvalidConfiguration,
    MR_SpliceFailedUnknownFailure,
    MR_SpliceCollision,
    MR_NoInsertionChannelFound,
    MR_NoPrimaryChannelFound,
    MR_SpliceRequestWasTooLate,
    MR_NoSplicePointWasFound,
    MR_SpliceQueueFull,
    MR_SessionPlaybackSuspect,
    MR_InsertionAborted,
    MR_InvalidCueMessage,
    MR_SplicingDeviceDoesNotExist,
    MR_InitRequestRefused,
    MR_UnknownMessageID,
    MR_InvalidSessionID,
    MR_SessionDidNotComplete,
    MR_InvalidRequestMessage,
    MR_DescriptorNotImplemented,
    MR_ChannelOverride,
    MR_InsertionChannelStartedEarly,
    MR_PlaybackRateBelowThreshold,
    MR_PmtChanged,
    MR_InvalidMessageSize,
    MR_InvalidMessageSyntax,
    MR_PortCollisionError,
    MR_SpliceFailedEasActive,
    MR_InsertionComponentNotFound,
    MR_ResourcesNotAvailable,
    MR_ComponentMismatch,
    MR_NA = 0xFFFF
  };

protected:
  MessageId_t m_messageId;
  quint16 m_messageSize;
  MessageResult_t m_result;
  quint16 m_resultExtension;
  CBaseMessage(MessageId_t messageId);
  virtual quint16 getDataSize() const {Q_ASSERT(false); return 0;}
public:
  CBaseMessage();
  CBaseMessage(const CBaseMessage& other);
  MessageId_t getMessageId() const { return m_messageId; }
  quint16 getMessageSize() const { return m_messageSize; }
  MessageResult_t getResult() const { return m_result; }
  virtual void write(QIODevice& ioDevice) const;
  virtual void read(QIODevice& ioDevice);
  virtual QString toString() const;
  static QString toString(MessageId_t v);
  static QString toString(MessageResult_t v);
};
QTextStream &operator<<(QTextStream& dbg, CBaseMessage::MessageId_t v);
QTextStream &operator<<(QTextStream& dbg, CBaseMessage::MessageResult_t v);


class CHardwareConfig: public QObject
{
  Q_OBJECT
  Q_ENUMS(LogicalMultiplexType_t)
public:
  enum LogicalMultiplexType_t
  {
    LMT_NotUsed = 0,
    LMT_UserDefined,
    LMT_MacAddress,
    LMT_IpV4Address,
    LMT_IpV6Address,
    LMT_AtmAddress,
    LMT_IpV4AddressWithSpts,
    LMT_IpV6AddressWithSpts
  };
private:
  quint16 m_length;
  quint16 m_chassis;
  quint16 m_card;
  quint16 m_port;
  LogicalMultiplexType_t m_logicalMultiplexType;
  // optional LogicalMultiplex can go here
public:
  CHardwareConfig();
  void write(QIODevice& ioDevice) const;
  quint16 getDataSize() const { return 2 * 5; }
  void setChassis(quint16 chassis) { m_chassis = chassis; }
  void setCard(quint16 card) { m_card = card; }
  void setPort(quint16 port) { m_port = port; }
  virtual QString toString() const;
};

class CInitRequestMessage: public CBaseMessage
{
  Q_OBJECT
private:
  quint16 m_version;
  QByteArray m_channelName;
  QByteArray m_splicerName;
  CHardwareConfig m_hardwareConfig;
  vector<CSpliceApiDescriptor> m_spliceApiDescriptors;
  virtual quint16 getDataSize() const;

public:
  CInitRequestMessage();
  virtual void write(QIODevice& ioDevice) const;
  virtual QString toString() const;
  void setChannelName(QString channelName);
  void setSplicerName(QString splicerName);
  void setChassis(quint16 chassis) { m_hardwareConfig.setChassis(chassis); }
  void setCard(quint16 card) { m_hardwareConfig.setCard(card); }
  void setPort(quint16 port) { m_hardwareConfig.setPort(port); }
};

class CInitResponseMessage: public CBaseMessage
{
  Q_OBJECT
public:
  quint16 m_version;
  QByteArray m_channelName;
public:
  CInitResponseMessage(const CBaseMessage& baseMessage);
  virtual void read(QIODevice& ioDevice);
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; } // can be 0 since the message will only be read
};

class CSpliceApiDescriptor
{
public:
  quint8 m_spliceDescriptorTag;
  quint8 m_descriptorLength;
  quint32 m_spliceApiIdentifier;
  vector<quint8> m_data;
public:
  CSpliceApiDescriptor(quint8 tag, vector<quint8> data);
  quint16 getSize() const;
  void write(QIODevice& ioDevice) const;
  QString toString() const;
};

class CTime
{
public:
  quint32 m_seconds;
  quint32 m_microseconds;
public:
  CTime();
  void read(QIODevice& ioDevice);
  void write(QIODevice& ioDevice) const;
  virtual QString toString() const;
  void fromTime(const QDateTime& dateTime);
  QDateTime toDateTime() const;
  quint16 getSize() const;
};

class CSpliceTime
{
public:
  bool m_timeSpecifiedFlag;
  uint64_t m_ptsTime;
public:
  CSpliceTime();
  CSpliceTime(bool timeSpecifiedFlag, uint64_t ptsTime);
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
};

class CSpliceDescriptor
{
public:
  quint8 m_spliceDescriptorTag;
  quint8 m_descriptorLength;
  quint32 m_identifier;
  vector<quint8> m_data;
public:
  CSpliceDescriptor(){}
  void read(QIODevice& ioDevice);
  QString toString() const;
};

class CSpliceSchedule
{
public:
  CSpliceSchedule(){}
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
};

class CComponent
{
public:
  quint8 m_componentTag;
  CSpliceTime m_spliceTime;
public:
  CComponent(){}
  void read(QIODevice& ioDevice, bool readTime);
  virtual QString toString() const;
};

class CBreakDuration
{
public:
  bool m_autoReturn;
  uint64_t m_duration;
public:
  CBreakDuration(){}
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
};

class CSpliceInsert
{
public:
  quint32 m_spliceEventId;
  bool m_spliceEventCancelIndicator;
  bool m_outOfNetworkIndicator;
  bool m_programSpliceFlag;
  bool m_durationFlag;
  bool m_spliceImmediateFlag;
  CSpliceTime m_spliceTime;
  quint8 m_componentCount;
  vector<CComponent> m_components;
  CBreakDuration m_breakDuration;
  quint16 m_uniqueProgramId;
  quint8 m_availNum;
  quint8 m_availsExpected;
public:
  CSpliceInsert(){}
  void read(QIODevice& ioDevice);
  QString toString() const;
};

class CSpliceInfoSection: public QObject
{
  Q_OBJECT
  Q_ENUMS(SpliceCommandType_t)
public:
  enum SpliceCommandType_t
  {
    SpliceNull = 0,
    Reserved1,
    Reserved2,
    Reserved3,
    SpliceSchedule,
    SpliceInsert,
    TimeSignal,
    BandwidthReservation
  };
public:
  quint8 m_tableId;
  bool m_sectionSyntaxIndicator;
  bool m_privateIndicator;
  quint16 m_sectionLength;
  quint8 m_protocolVersion;
  bool m_encryptedPacket;
  quint8 m_encryptionAlgorithm;
  uint64_t m_ptsAdjustment;
  quint8 m_cwIndex;
  quint16 m_spliceCommandLength;
  SpliceCommandType_t m_spliceCommandType;
  CSpliceSchedule m_spliceSchedule;
  CSpliceInsert m_spliceInsert;
  CSpliceTime m_timeSignal;
  quint16 m_descriptorLoopLength;
  vector<CSpliceDescriptor> m_spliceDescriptors;
  quint32 m_eCrc;
  quint32 m_crc;
public:
  CSpliceInfoSection();
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
  static QString toString(SpliceCommandType_t v);
};
QTextStream &operator<<(QTextStream& dbg, CSpliceInfoSection::SpliceCommandType_t v);


class CCueRequestMessage: public CBaseMessage
{
  Q_OBJECT
public:
  CTime m_time;
  CSpliceInfoSection m_spliceInfoSection;
public:
  CCueRequestMessage(const CBaseMessage& baseMessage);
  virtual void read(QIODevice& ioDevice);
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; } // unused since the message will not be written
};

class CCueResponseMessage: public CBaseMessage
{
  Q_OBJECT
public:
  CCueResponseMessage(): CBaseMessage(MID_CueResponse){}
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; }
};

class CGeneralMessage: public CBaseMessage
{
  Q_OBJECT
public:
  CGeneralMessage(): CBaseMessage(MID_GeneralResponse){}
  CGeneralMessage(const CBaseMessage& baseMessage);
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; }
};


class CDescriptor
{
public:
  quint8 m_tag;
  QList<quint8> m_data;
public:
  CDescriptor(quint8 tag, QList<quint8> data);
  CDescriptor();
  void write(QIODevice& ioDevice) const;
  QString toString() const;
  quint16 getDataSize() const;
  QString dataToString() const;
  bool dataFromString(const QString& string);
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
private:
  quint8 getSize() const;

};

class CSpliceElementaryStream
{
public:
  quint16 m_pid;
  quint16 m_streamType;
  quint32 m_avgBitrate;
  quint32 m_maxBitrate;
  quint32 m_minBitrate;
  quint16 m_hResolution;
  quint16 m_vResolution;
  QList<CDescriptor> m_descriptors;
public:
  CSpliceElementaryStream();
  CSpliceElementaryStream(quint16 pid, quint16 streamType, quint32 avgBitrate = 0xFFFFFFFF,
               quint32 maxBitrate = 0xFFFFFFFF, quint32 minBitrate = 0xFFFFFFFF,
               quint16 hResolution = 0xFFFF, quint16 vResolution = 0xFFFF);
  void addDescriptor(const CDescriptor& descriptor);
  void addDescriptor(quint8 tag, QList<quint8> data);
  void write(QIODevice& ioDevice) const;
  QString toString() const;
  quint8 getDataSize() const;
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
};

class CSpliceRequestMessage: public CBaseMessage
{
  Q_OBJECT
public:
  quint32 m_sessionId;
  quint32 m_priorSession;
  CTime m_time;
  quint16 m_serviceId;
  quint16 m_pcrPid;
  QList<CSpliceElementaryStream> m_spliceElementaryStreams;
  quint32 m_duration;
  quint32 m_spliceEventId;
  quint32 m_postBlack;
  quint8 m_accessType;
  quint8 m_overridePlaying;
  quint8 m_returnToPriorChannel;
  vector<CSpliceApiDescriptor> m_spliceApiDescriptors;
public:
  CSpliceRequestMessage(): CBaseMessage(MID_SpliceRequest){}
  void write(QIODevice& ioDevice) const;
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const;
};

class CAliveRequestMessage: public CBaseMessage
{
  Q_OBJECT
public:
  CTime m_time;
public:
  CAliveRequestMessage();
  void write(QIODevice& ioDevice) const;
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const;
};

class CAliveResponseMessage: public CBaseMessage
{
  Q_OBJECT
  Q_ENUMS(AliveState_t)
public:
  enum AliveState_t
  {
    NoOutput,
    OnPrimary,
    OnInsertion
  };

public:
  AliveState_t m_aliveState;
  quint32 m_sessionId;
  CTime m_time;
public:
  CAliveResponseMessage(const CBaseMessage& baseMessage): CBaseMessage(baseMessage){}
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
  static QString toString(AliveState_t v);
private:
  virtual quint16 getDataSize() const;
};

QTextStream &operator<<(QTextStream& dbg, CAliveResponseMessage::AliveState_t v);


class CSpliceResponseMessage: public CBaseMessage
{
  Q_OBJECT
public:
  CSpliceResponseMessage(const CBaseMessage& baseMessage);
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; }
};

class CSpliceCompleteResponseMessage: public CBaseMessage
{
  Q_OBJECT
  Q_ENUMS(SpliceTypeFlag_t)
public:
  enum SpliceTypeFlag_t
  {
   SpliceIn,
   SpliceOut
  };
public:
  quint32 m_sessionId;
  SpliceTypeFlag_t m_spliceTypeFlag;
  quint32 m_bitrate;
  quint32 m_playedDuration;

public:
  CSpliceCompleteResponseMessage(const CBaseMessage& baseMessage);
  void read(QIODevice& ioDevice);
  virtual QString toString() const;
private:
  virtual quint16 getDataSize() const { return 0; }
};

#endif // SCTE30MESSAGES_H
