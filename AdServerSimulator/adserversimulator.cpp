#include "adserversimulator.h"
#include "ui_adserversimulator.h"
#include "adchannel.h"
#include "channelconfig.h"
#include "channellogview.h"
#include "channelconfigview.h"
#include "channelconfigitem.h"
#include <QDebug>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

AdServerSimulator::AdServerSimulator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdServerSimulator),
    m_currentDir(QDir::currentPath())
{
  ui->setupUi(this);
  ui->channels->setLayout(new QVBoxLayout(ui->channels));
}

AdServerSimulator::~AdServerSimulator()
{
  delete ui;
}

void AdServerSimulator::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Settings"),m_currentDir, tr("XML files (*.xml);;All files (*.*"));
  if (fileName.isEmpty())
  {
    return;
  }
  m_currentDir = QFileInfo(fileName).absolutePath();
  qDebug() << "Clean path" << m_currentDir;
  if (!readChannelSettings(fileName))
  {
    return;
  }
  // remove previous channels
  for (QList<ChannelConfigItem*>::iterator it = m_channelConfigItems.begin(); it != m_channelConfigItems.end(); ++it)
  {
    ui->channels->layout()->removeWidget(*it);
    delete *it;
  }
  m_channelConfigItems.clear();
  addReadChannelConfigItems();
}

void AdServerSimulator::addReadChannelConfigItems()
{
  for (QList<CChannelConfig>::iterator it = m_readChannelConfigs.begin(); it != m_readChannelConfigs.end(); ++it)
  {
    ChannelConfigItem* item = new ChannelConfigItem(*it, this);
    connect(item, SIGNAL(removed(ChannelConfigItem*)), this, SLOT(removeChannel(ChannelConfigItem*)));
    ui->channels->layout()->addWidget(item);
    m_channelConfigItems.push_back(item);
  }
}


bool AdServerSimulator::readChannelSettings(QString fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("Read Settings"), tr("Cannot read file %1.").arg(file.errorString()));
    return false;
  }
  QXmlStreamReader reader(&file);
  reader.readNextStartElement();
  // reader.readElementText();
  if (reader.name() != "channelsettings")
  {
    QMessageBox::warning(this, tr("Read Settings"), QString("Invalid file. No channelsettings element found"));
    return false;
  }
  if (reader.attributes().value("version") != "1.0")
  {
    QMessageBox::warning(this, tr("Read Settings"), QString("Wrong version"));
    return false;
  }
  reader.readNext();
  if (!readChannelConfigs(reader))
  {
    QMessageBox::warning(this, tr("Read Settings"), QString("Error on line %1 col %2").arg(QString::number(reader.lineNumber()), QString::number(reader.columnNumber())));
    return false;
  }
  return true;
}

bool AdServerSimulator::readChannelConfigs(QXmlStreamReader& reader)
{
  m_readChannelConfigs.clear();
  while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
          reader.name() == "channelsettings"))
  {
    if(reader.tokenType() == QXmlStreamReader::StartElement)
    {

      if (reader.name() == "channelConfig")
      {
        CChannelConfig newChannel;
        if (!newChannel.fromXml(reader))
        {
          return false;
        }
        m_readChannelConfigs.push_back(newChannel);
      }
      else
      {
        qWarning() << "ChannelSettings: Unknown element in XML file: " << reader.name();
        return false;
      }
    }
    reader.readNext();
  }
  return true;
}

void AdServerSimulator::addFromFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Add File"),m_currentDir, tr("XML files (*.xml);;All files (*.*"));
  if (fileName.isEmpty())
  {
    return;
  }
  m_currentDir = QFileInfo(fileName).absolutePath();
  if (!readChannelSettings(fileName))
  {
    return;
  }
  addReadChannelConfigItems();
}

void AdServerSimulator::saveFile()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Settings"),m_currentDir, tr("XML files (*.xml);;All files (*.*"));
  if (fileName.isEmpty())
  {
    return;
  }
  m_currentDir = QFileInfo(fileName).absolutePath();
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("Save Settings"), tr("Cannot write file %1.").arg(file.errorString()));
    return;
  }
  QXmlStreamWriter writer(&file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeDTD("<!DOCTYPE channelsettings>");
  writer.writeStartElement("channelsettings");
  writer.writeAttribute("version", "1.0");
  for (QList<ChannelConfigItem*>::const_iterator it = m_channelConfigItems.begin(); it != m_channelConfigItems.end(); ++it)
  {
    (*it)->getChannelConfig().toXml(writer);
  }
  writer.writeEndElement();
  writer.writeEndDocument();
}

void AdServerSimulator::addChannel()
{
  ChannelConfigItem* item = new ChannelConfigItem(CChannelConfig(), this);
  connect(item, SIGNAL(removed(ChannelConfigItem*)), this, SLOT(removeChannel(ChannelConfigItem*)));
  ui->channels->layout()->addWidget(item);
  m_channelConfigItems.push_back(item);
}

void AdServerSimulator::removeChannel(ChannelConfigItem* item)
{
  int index = ui->channels->layout()->indexOf(item);
  m_channelConfigItems.removeAt(index);
  ui->channels->layout()->removeWidget(item);
}

void AdServerSimulator::connectAll()
{
  for (QList<ChannelConfigItem*>::const_iterator it = m_channelConfigItems.begin(); it != m_channelConfigItems.end(); ++it)
  {
    (*it)->connectSplicer();
  }
}

void AdServerSimulator::disconnectAll()
{
  for (QList<ChannelConfigItem*>::const_iterator it = m_channelConfigItems.begin(); it != m_channelConfigItems.end(); ++it)
  {
    (*it)->disconnectSplicer();
  }}

void AdServerSimulator::openLog()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log"),m_currentDir, tr("XML files (*.xml);;All files (*.*"));
  if (fileName.isEmpty())
  {
    return;
  }
  m_currentDir = QFileInfo(fileName).absolutePath();
  CAdChannel* adChannel = new CAdChannel();
  if (adChannel->loadLog(fileName))
  {
    ChannelConfigItem* item = new ChannelConfigItem(adChannel, this);
    //item->SetAdChannel(adChannel);
    connect(item, SIGNAL(removed(ChannelConfigItem*)), this, SLOT(removeChannel(ChannelConfigItem*)));
    ui->channels->layout()->addWidget(item);
    m_channelConfigItems.push_back(item);
  }
}
