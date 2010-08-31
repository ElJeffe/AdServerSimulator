#ifndef SPLICESESSIONS_H
#define SPLICESESSIONS_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "helpers.h"

class QTextStream;
class QTimer;
class QXmlStreamWriter;
class CChannelLogger;

class CSpliceSession: public QObject
{
  Q_OBJECT
  Q_ENUMS(SessionState_t)
public:
  enum SessionState_t
  {
    None,
    Initializing,
    SplicedIn,
    Finished,
    Failed
  };

private:
  quint32 m_sessionId;
  quint32 m_priorSessionId;
  SessionState_t m_sessionState;
  quint32 m_expectedDuration;
  quint32 m_duration;
  QDateTime m_startTime;
  QTimer* m_durationTimer;
  CChannelLogger* m_channelLogger;
public:
  CSpliceSession(quint32 sessionId, quint32 priorSessionId, const QDateTime& startTime, CChannelLogger* channelLogger, QObject* parent = 0);
  CSpliceSession(CChannelLogger* channelLogger, QObject* parent = 0);
  void setNewState(SessionState_t newState);
  quint32 getSessionId() const { return m_sessionId; }
  quint32 getPriorSessionId() const { return m_priorSessionId; }
  quint32 getExpectedDuration() const { return m_expectedDuration; }
  quint32 getDuration() const { return m_duration; }
  const QDateTime& getStartTime() const { return m_startTime; }
  SessionState_t getSessionState() const { return m_sessionState; }
  void setExpectedDuration(quint32 expectedDuration);
  void setDuration(quint32 duration);
  static QString toString(SessionState_t sessionState);
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
private slots:
  void durationTimerExpired();
signals:
  void dataChanged(CSpliceSession* spliceSession);
};
QTextStream &operator<<(QTextStream& dbg, CSpliceSession::SessionState_t v);


class CSpliceSessions: public QObject
{
  Q_OBJECT
private:
  quint16 m_currentSessionId;
  QMap<quint32, CSpliceSession*> m_spliceSessions;
  QList<CSpliceSession*> m_spliceSessionsList;
  CChannelLogger* m_channelLogger;
public:
  CSpliceSessions(CChannelLogger* channelLogger, QObject* parent = 0);
  CSpliceSession* createSpliceSession(const QDateTime& startTime, quint32 priorSession = 0xFFFFFFFF);
  CSpliceSession* getSpliceSession(quint32 sessionId) const;
  CSpliceSession* at(int pos) const;
  QString toString() const;
  int count() const;
  quint16 getLastSessionId() const { return m_currentSessionId; }
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
private slots:
  void dataChanged(CSpliceSession* spliceSession);
signals:
  void sessionChanged(int pos);
  void sessionAdded(int pos);
};
#endif // SPLICESESSIONS_H
