#ifndef HELPERS_H
#define HELPERS_H

#include <QTime>

class QXmlStreamWriter;
class QXmlStreamReader;

class Helpers
{
private:
  Helpers();
public:
  static QTime ticksToTime(quint32 ticks);
  static int timeToTicks(const QTime& time);
  static QString ticksToString(quint32 ticks);
  static void toXml(QXmlStreamWriter& writer, const QString& name, const QString& value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, int value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, quint32 value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, bool value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, const QDateTime& value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, const QDate& value);
  static void toXml(QXmlStreamWriter& writer, const QString& name, const QTime& value);
  static bool fromXml(QXmlStreamReader& reader, QString& value);
  static bool fromXml(QXmlStreamReader& reader, int& value);
  static bool fromXml(QXmlStreamReader& reader, quint32& value);
  static bool fromXml(QXmlStreamReader& reader, quint16& value);
  static bool fromXml(QXmlStreamReader& reader, quint8& value);
  static bool fromXml(QXmlStreamReader& reader, bool& value);
  static bool fromXml(QXmlStreamReader& reader, const QString& name, QDateTime& value);
  static bool fromXml(QXmlStreamReader& reader, const QString& name, QDate& value);
  static bool fromXml(QXmlStreamReader& reader, const QString& name, QTime& value);
};

#endif // HELPERS_H
