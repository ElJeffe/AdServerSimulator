#include "helpers.h"
#include <QXmlStreamWriter>
#include <QDebug>

Helpers::Helpers()
{
}

QTime Helpers::ticksToTime(quint32 ticks)
{
  QTime time = QTime().addMSecs(ticks / 90);
  return time;
}

int Helpers::timeToTicks(const QTime& time)
{
  if (time > QTime().addSecs(47721))
  {
    return 0xFFFFFFFE;
  }
  int totalSec = time.hour() * 3600 + time.minute() * 60 + time.second();
  return (totalSec * 90000) + (time.msec() * 9 / 100);
}

QString Helpers::ticksToString(quint32 ticks)
{
  if (ticks == 0xFFFFFFFF)
  {
    return QString::number(ticks);
  }
  int ms = QTime().msecsTo(ticksToTime(ticks));
  return QString("%1 (%2 ms)").arg(QString::number(ticks), QString::number(ms));
}

void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, const QString& value)
{
  writer.writeTextElement(name, value);
}

bool Helpers::fromXml(QXmlStreamReader& reader, QString& value)
{
  value = reader.readElementText();
  return true;
}

void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, quint32 value)
{
  writer.writeTextElement(name, QString::number(value));
}

bool Helpers::fromXml(QXmlStreamReader& reader, quint32& value)
{
  QString str;
  bool res;
  fromXml(reader, str);
  value = str.toUInt(&res);
  return res;
}

bool Helpers::fromXml(QXmlStreamReader& reader, quint16& value)
{
  quint32 v;
  bool res = fromXml(reader, v);
  value = (quint16)v;
  return res;
}

bool Helpers::fromXml(QXmlStreamReader& reader, quint8& value)
{
  quint32 v;
  bool res = fromXml(reader, v);
  value = (quint8)v;
  return res;
}

void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, int value)
{
  writer.writeTextElement(name, QString::number(value));
}

bool Helpers::fromXml(QXmlStreamReader& reader, int& value)
{
  QString str;
  bool res;
  fromXml(reader, str);
  value = str.toInt(&res);
  return res;
}

void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, bool value)
{
  writer.writeTextElement(name, QString::number(value));
}

bool Helpers::fromXml(QXmlStreamReader& reader, bool& value)
{
  QString str;
  bool res;
  fromXml(reader, str);
  value = str.toShort(&res);
  return res;
}

void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, const QDateTime& value)
{
  writer.writeStartElement(name);
  toXml(writer, "date", value.date());
  toXml(writer, "time", value.time());
  writer.writeEndElement();
}

bool Helpers::fromXml(QXmlStreamReader& reader, const QString& name, QDateTime& value)
{
  // check that it is really a good element
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != name)
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == name))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "date")
      {
        QDate date;
        if (!Helpers::fromXml(reader, "date", date))
          return false;
        value.setDate(date);
      }
      else if (reader.name() == "time")
      {
        QTime time;
        if (!Helpers::fromXml(reader, "time", time))
          return false;
        value.setTime(time);
      }
      else
      {
        qWarning() << "QDateTime: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}


void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, const QDate& value)
{
  writer.writeStartElement(name);
  toXml(writer, "year", value.year());
  toXml(writer, "month", value.month());
  toXml(writer, "day", value.day());
  writer.writeEndElement();
}

bool Helpers::fromXml(QXmlStreamReader& reader, const QString& name, QDate& value)
{
  int year, month, day;
  // check that it is really a good element
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != name)
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == name))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "year")
      {
        if (!Helpers::fromXml(reader, year))
          return false;
      }
      else if (reader.name() == "month")
      {
        if (!Helpers::fromXml(reader, month))
          return false;
      }
      else if (reader.name() == "day")
      {
        if (!Helpers::fromXml(reader, day))
          return false;
      }
      else
      {
        qWarning() << "QDate: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  value.setYMD(year, month, day);
  return true;
}


void Helpers::toXml(QXmlStreamWriter& writer, const QString& name, const QTime& value)
{
  writer.writeStartElement(name);
  toXml(writer, "hour", value.hour());
  toXml(writer, "minute", value.minute());
  toXml(writer, "second", value.second());
  toXml(writer, "msec", value.msec());
  writer.writeEndElement();
}

bool Helpers::fromXml(QXmlStreamReader& reader, const QString& name, QTime& value)
{
  int h, m, s, ms;
  // check that it is really a good element
  if(reader.tokenType() != QXmlStreamReader::StartElement ||
     reader.name() != name)
  {
    return false;
  }
  reader.readNext();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == name))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {
      if (reader.name() == "hour")
      {
        if (!Helpers::fromXml(reader, h))
          return false;
      }
      else if (reader.name() == "minute")
      {
        if (!Helpers::fromXml(reader, m))
          return false;
      }
      else if (reader.name() == "second")
      {
        if (!Helpers::fromXml(reader, s))
          return false;
      }
      else if (reader.name() == "msec")
      {
        if (!Helpers::fromXml(reader, ms))
          return false;
      }
      else
      {
        qWarning() << "QTime: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  value.setHMS(h, m, s, ms);
  return true;
}



