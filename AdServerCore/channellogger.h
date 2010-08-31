#ifndef CHANNELLOGGER_H
#define CHANNELLOGGER_H

#include <QObject>
#include <QDateTime>

class QXmlStreamWriter;
class QXmlStreamReader;

class CLogLine: public QObject
{
  Q_OBJECT
  Q_ENUMS(LogLevel_t)
public:
      enum LogLevel_t
  {
    Info,
    Warning,
    Error,
    MessageRcv,
    MessageSend
  };
private:
  QDateTime m_time;
  LogLevel_t m_logLevel;
  QString m_log;
public:
  CLogLine(LogLevel_t logLevel, const QString& log, QObject *parent = 0);
  CLogLine(QObject *parent = 0);
  const QDateTime& getTime() const { return m_time; }
  LogLevel_t getLogLevel() const { return m_logLevel; }
  const QString& getLog() const { return m_log; }
  static QString toString(LogLevel_t logLevel);
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);
};

class CChannelLogger : public QObject
{
  Q_OBJECT

private:
  QList<CLogLine*> m_logLines;
public:
  explicit CChannelLogger(QObject *parent = 0);
  void log(CLogLine::LogLevel_t logLevel, const QString& log);
  int count() const;
  const CLogLine* at(int pos) const;
  void toXml(QXmlStreamWriter& writer) const;
  bool fromXml(QXmlStreamReader& reader);

public slots:
  void clear();

signals:
  void logAdded(int);
};

#endif // CHANNELLOGGER_H
