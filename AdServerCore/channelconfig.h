#ifndef CHANNELCONFIG_H
#define CHANNELCONFIG_H

#include <QString>
#include <QDateTime>
#include <QList>
#include "scte30messages.h"

class QTimer;
class QXmlStreamWriter;
class QXmlStreamReader;

class CAdConfig
{
public:
  quint16 m_serviceId;
  quint16 m_pcrPid;
  QList<CSpliceElementaryStream> m_spliceElementaryStreams;
  quint32 m_duration;
  quint32 m_postBlack;
  quint8 m_accessType;
  bool m_overridePlaying;
  bool m_returnToPriorChannel;

  CAdConfig();
  CAdConfig(quint16 pcrPid, quint16 serviceId = 0xFFFF, quint32 durationInTicks = 0xFFFFFFFF,
            quint32 postBlackInTicks = 0xFFFFFFFF, quint8 accessType = 0, bool overridePlaying = false,
            bool returnToPriorChannel = true);
  void setDuration(const QTime& duration);
  void setDurationInTicks(quint16 duration) { m_duration = duration; }
  void setPostBlack(const QTime& postBlack);
  void setPostBlackInTicks(quint16 postBlack) { m_postBlack = postBlack; }
  QTime getTotalDuration() const;
  void addElementaryStream(const CSpliceElementaryStream& spliceElementaryStream);
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
};

class CAdBlock: public QObject
{
  Q_OBJECT
public:
  QString m_name;
  QDateTime m_startTime;
  QTime m_recurringTime;
  QList<CAdConfig> m_adConfigs;

  QTimer* m_adTimer;
  QTimer* m_b2bAdTimer;
  QDateTime m_nextAdTime;
  QDateTime m_nextB2bAdTime;
  int m_nextB2bIndex;
private:
  int m_secondsBeforeSpliceRequest;
public:
  CAdBlock(const QDateTime& startTime = QDateTime(), const QTime& recurringTime = QTime(), const QString& name = "adBlock");
  CAdBlock(const CAdBlock& other);
  CAdBlock& operator=(const CAdBlock& other);
  ~CAdBlock();
  void addAdConfig(const CAdConfig& adConfig);
  void startTimer(int m_secondsBeforeSpliceRequest);
  void stopTimer();
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
  void setScte35StartTime(QDateTime firstAdStartTime);
private:
  void startB2bTimer();
  void startB2bTimer(QDateTime b2bAdTime);
signals:
  void startAd(const CAdConfig& adConfig, const QDateTime& startTime);
  void startB2bAd(const CAdConfig& adConfig, const QDateTime& startTime);
private slots:
  void adTimerExpired();
  void b2bAdTimerExpired();
};

class CChannelConfig
{
public:
  QString m_splicerAddress;
  QString m_channelName;
  QString m_splicerName;
  int m_chassis;
  int m_card;
  int m_port;
  int m_secondsBeforeSpliceRequest;
  bool m_processScte35;
  QList<CAdBlock> m_adBlocks;
public:
  CChannelConfig();
  CChannelConfig(const QString& splicerAddress, const QString& channelName, const QString& splicerName,
                 int chassis, int card, int port, int secondsBeforeSpliceRequest = 3, bool processScte35 = true);
  void addAdBlock(const CAdBlock& adBlock);
  void startTimers();
  void stopTimers();
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
  QString getSplicerAddress() const { return m_splicerAddress; }
  QString getChannelName() const { return m_channelName; }
  QString getSplicerName() const { return m_splicerName; }
};

#endif // CHANNELCONFIG_H
