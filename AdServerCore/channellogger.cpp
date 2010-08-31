#include "channellogger.h"
#include <QMetaEnum>
#include <QDebug>
#include <QXmlStreamWriter>
#include "helpers.h"

CLogLine::CLogLine(LogLevel_t logLevel, const QString& log, QObject* parent):
  QObject(parent),
  m_time(QDateTime().currentDateTime()),
  m_logLevel(logLevel),
  m_log(log)
{
}

CLogLine::CLogLine(QObject* parent):
  QObject(parent)
{
}
QString CLogLine::toString(LogLevel_t v)
{
  int enumIndex = CLogLine::staticMetaObject.indexOfEnumerator("LogLevel_t");
  QMetaEnum metaEnum = CLogLine::staticMetaObject.enumerator(enumIndex);
  return metaEnum.valueToKey(v);
}

void CLogLine::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("logLine");
  Helpers::toXml(writer, "time", m_time);
  Helpers::toXml(writer, "logLevel", (int)m_logLevel);
  Helpers::toXml(writer, "log", m_log);
  writer.writeEndElement();
}
bool CLogLine::fromXml(QXmlStreamReader& reader)
{
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "logLine")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "logLine"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "time")
      {
        if (!Helpers::fromXml(reader, "time", m_time))
          return false;
      }
      else if (reader.name() == "logLevel")
      {
        int logLevel;
        if (!Helpers::fromXml(reader, logLevel))
          return false;
        m_logLevel = (CLogLine::LogLevel_t)logLevel;
      }
      else if (reader.name() == "log")
      {
        if (!Helpers::fromXml(reader, m_log))
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

CChannelLogger::CChannelLogger(QObject *parent) :
    QObject(parent)
{
}

void CChannelLogger::log(CLogLine::LogLevel_t logLevel, const QString& log)
{
  if (logLevel != CLogLine::MessageRcv && logLevel != CLogLine::MessageSend)
  {
    qDebug() << QTime().currentTime().toString("hh'h' mm'm' ss's' zzz'ms'") << log;
  }
  m_logLines.append(new CLogLine(logLevel, log, this));
  logAdded(m_logLines.size() - 1);
}

int CChannelLogger::count() const
{
  return m_logLines.count();
}

const CLogLine* CChannelLogger::at(int pos) const
{
  return m_logLines.at(pos);
}

void CChannelLogger::clear()
{
  m_logLines.clear();
}

void CChannelLogger::toXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("channelLogger");
  for (QList<CLogLine*>::const_iterator it = m_logLines.begin();
    it != m_logLines.end(); ++it)
  {
    (*it)->toXml(writer);
  }
  writer.writeEndElement();
}

bool CChannelLogger::fromXml(QXmlStreamReader& reader)
{
  m_logLines.clear();
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != "channelLogger")
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "channelLogger"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "logLine")
      {
        CLogLine* line = new CLogLine(this);
        if (!line->fromXml(reader))
          return false;
        m_logLines.append(line);

      }
      else
      {
        qWarning() << "SpliceSessions: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;}


