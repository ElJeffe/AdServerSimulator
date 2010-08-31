#include "splicesessions.h"
#include <QTextStream>
#include <QDebug>
#include <QMetaEnum>
#include <QTimer>
#include <QXmlStreamWriter>
#include "channellogger.h"


#define DURATION_EXTRA_TIME 3
#define INIT_EXTRA_TIME 3
/**************************************************************************
 * CSpliceSession
 **************************************************************************/

CSpliceSession::CSpliceSession(quint32 sessionId, quint32 priorSessionId, const QDateTime& startTime, CChannelLogger* channelLogger, QObject *parent):
    QObject(parent),
    m_sessionId(sessionId),
    m_priorSessionId(priorSessionId),
    m_sessionState(CSpliceSession::None),
    m_expectedDuration(0),
    m_duration(0),
    m_startTime(startTime),
    m_channelLogger(channelLogger)
{
}

CSpliceSession::CSpliceSession(CChannelLogger* channelLogger, QObject *parent):
    QObject(parent),
    m_sessionId(0),
    m_priorSessionId(0xFFFFFFFF),
    m_sessionState(CSpliceSession::None),
    m_expectedDuration(0),
    m_duration(0),
    m_startTime(QDateTime()),
    m_channelLogger(channelLogger)
{
}

void CSpliceSession::setNewState(SessionState_t newState)
{
  if (newState == m_sessionState)
    return;
  if (m_sessionState == Failed)
    return;
  if (newState < m_sessionState)
  {
    m_channelLogger->log(CLogLine::Error, QString("New state(%1) can not follow old state(%2)").arg(toString(newState), toString(m_sessionState)));
    return;
  }
  m_sessionState = newState;
  switch (newState)
  {
  case Initializing:
    {
      m_durationTimer = new QTimer(this);
      m_durationTimer->setSingleShot(true);
      connect(m_durationTimer, SIGNAL(timeout()), this, SLOT(durationTimerExpired()));
      int secToSplicePoint = QDateTime().currentDateTime().toUTC().secsTo(m_startTime) + INIT_EXTRA_TIME;
      m_channelLogger->log(CLogLine::Warning, QString("Splice should start in %1").arg(QString::number(secToSplicePoint)));
      m_durationTimer->start((secToSplicePoint) * 1000);
      break;
    }
  case SplicedIn:
    {
      m_durationTimer->stop();
      int secToEnd = QTime().secsTo(Helpers::ticksToTime(m_expectedDuration)) + DURATION_EXTRA_TIME;
      m_channelLogger->log(CLogLine::Warning, QString("Splice should stop in %1").arg(QString::number(secToEnd)));
      m_durationTimer->start(secToEnd * 1000);
      break;
    }
  case Finished:
    {
      m_durationTimer->stop();
      break;
    }
  default:
    break;
  }
  dataChanged(this);
}

void CSpliceSession::setDuration(quint32 duration)
{
  m_duration = duration;

  if (m_duration < m_expectedDuration || m_duration > m_expectedDuration + Helpers::timeToTicks(QTime(0,0,DURATION_EXTRA_TIME)))
  {
    qWarning() << "Expected duration is " << m_expectedDuration << " but the ad played for " << m_duration;
    m_sessionState = CSpliceSession::Failed;
  }
  dataChanged(this);
}

void CSpliceSession::setExpectedDuration(quint32 expectedDuration)
{
  m_expectedDuration = expectedDuration;
  dataChanged(this);
}

void CSpliceSession::durationTimerExpired()
{
  if (m_sessionState == Initializing)
  {
    m_channelLogger->log(CLogLine::Error, "The Ad did not start in time");
  }
  else
  {
    m_channelLogger->log(CLogLine::Error, "The Ad did not stop in time");
  }
  setNewState(Failed);
}


QString CSpliceSession::toString(SessionState_t v)
{
  int enumIndex = CSpliceSession::staticMetaObject.indexOfEnumerator("SessionState_t");
  QMetaEnum metaEnum = CSpliceSession::staticMetaObject.enumerator(enumIndex);
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
QTextStream& operator<<(QTextStream& dbg, CSpliceSession::SessionState_t v)
{
  dbg << CSpliceSession::toString(v);
  return dbg;
}

void CSpliceSession::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("spliceSession");
  Helpers::toXml(writer, "startTime", m_startTime);
  Helpers::toXml(writer, "sessionId", m_sessionId);
  Helpers::toXml(writer, "priorSessionId", m_priorSessionId);
  Helpers::toXml(writer, "sessionState", (int)m_sessionState);
  Helpers::toXml(writer, "expectedDuration", m_expectedDuration);
  Helpers::toXml(writer, "duration", m_duration);
  writer.writeEndElement();
}

bool CSpliceSession::fromXml(QXmlStreamReader& reader)
{
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "spliceSession")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "spliceSession"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "startTime")
      {
        if (!Helpers::fromXml(reader, "startTime", m_startTime))
          return false;
      }
      else if (reader.name() == "sessionId")
      {
        if (!Helpers::fromXml(reader, m_sessionId))
          return false;
      }
      else if (reader.name() == "priorSessionId")
      {
        if (!Helpers::fromXml(reader, m_priorSessionId))
          return false;
      }
      else if (reader.name() == "sessionState")
      {
        int state = (int)m_sessionState;
        if (!Helpers::fromXml(reader, state))
          return false;
        m_sessionState = (SessionState_t)state;
      }
      else if (reader.name() == "expectedDuration")
      {
        if (!Helpers::fromXml(reader, m_expectedDuration))
          return false;
      }
      else if (reader.name() == "duration")
      {
        if (!Helpers::fromXml(reader, m_duration))
          return false;
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
 * CSpliceSessions
 **************************************************************************/

CSpliceSessions::CSpliceSessions(CChannelLogger* channelLogger, QObject *parent):
    QObject(parent),
    m_channelLogger(channelLogger)
{
  m_currentSessionId = qrand();
}



CSpliceSession* CSpliceSessions::createSpliceSession(const QDateTime& startTime, quint32 priorSession)
{
  m_currentSessionId++;
  CSpliceSession* newSession = new CSpliceSession(m_currentSessionId, priorSession, startTime, m_channelLogger);
  newSession->setParent(this);
  connect(newSession, SIGNAL(dataChanged(CSpliceSession*)), this, SLOT(dataChanged(CSpliceSession*)));
  m_spliceSessions.insert(m_currentSessionId, newSession);
  m_spliceSessionsList.append(newSession);
  sessionAdded(m_spliceSessionsList.size() - 1);
  return newSession;
}

CSpliceSession* CSpliceSessions::getSpliceSession(quint32 sessionId) const
{
  if (!m_spliceSessions.contains(sessionId))
  {
    qWarning() << "The requested session ID does not exist: " << sessionId;
    qWarning() << toString();
    Q_ASSERT(false);
    return 0;
  }
  return m_spliceSessions[sessionId];
}

CSpliceSession* CSpliceSessions::at(int pos) const
{
  if (pos > m_spliceSessionsList.size())
    return 0;
  return m_spliceSessionsList.at(pos);
}


QString CSpliceSessions::toString() const
{
  QString string;
  QTextStream stream(&string);
  for (QMap<quint32, CSpliceSession*>::const_iterator it = m_spliceSessions.begin(); it != m_spliceSessions.end(); ++it)
  {
    CSpliceSession* spliceSession = it.value();
    stream << "SessionId: " << spliceSession->getSessionId() << " PriorSessionId: " << spliceSession->getPriorSessionId() << " State: " << spliceSession->getSessionState();
  }
  return string;
}

int CSpliceSessions::count() const
{
  return m_spliceSessionsList.count();
}

void CSpliceSessions::dataChanged(CSpliceSession* spliceSession)
{
  int pos = m_spliceSessionsList.indexOf(spliceSession);
  if (pos >=0)
  {
    sessionChanged(pos);
  }
}

void CSpliceSessions::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("spliceSessions");
  for (QList<CSpliceSession*>::const_iterator it = m_spliceSessionsList.begin();
    it != m_spliceSessionsList.end(); ++it)
  {
    (*it)->toXml(writer);
  }
  writer.writeEndElement();
}

bool CSpliceSessions::fromXml(QXmlStreamReader& reader)
{
  m_spliceSessionsList.clear();
  m_spliceSessions.clear();
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "spliceSessions")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "spliceSessions"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "spliceSession")
      {
        CSpliceSession* session = new CSpliceSession(m_channelLogger, this);
        if (!session->fromXml(reader))
          return false;
        m_spliceSessionsList.append(session);
        m_spliceSessions.insert(session->getSessionId(), session);
      }
      else
      {
        qWarning() << "SpliceSessions: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}


