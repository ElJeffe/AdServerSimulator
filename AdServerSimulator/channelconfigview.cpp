#include "channelconfigview.h"
#include "ui_channelconfigview.h"
#include "adblockitem.h"

ChannelConfigView::ChannelConfigView(const CChannelConfig& channelConfig,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelConfigView),
    m_channelConfig(channelConfig)
{
    ui->setupUi(this);

    ui->splicerAddress->setText(m_channelConfig.getSplicerAddress());
    ui->channelName->setText(m_channelConfig.getChannelName());
    ui->splicerName->setText(m_channelConfig.getSplicerName());
    ui->chassis->setText(QString::number(m_channelConfig.m_chassis));
    ui->card->setText(QString::number(m_channelConfig.m_card));
    ui->port->setText(QString::number(m_channelConfig.m_port));
    ui->timeBeforeSplice->setText(QString::number(m_channelConfig.m_secondsBeforeSpliceRequest));
    ui->processScte35->setChecked(m_channelConfig.m_processScte35);

    ui->adBlocks->setLayout(new QVBoxLayout(ui->adBlocks));
    for (QList<CAdBlock>::iterator it = m_channelConfig.m_adBlocks.begin(); it != m_channelConfig.m_adBlocks.end(); ++it)
    {
      AdBlockItem* adBlockItem = new AdBlockItem(&(*it), this);
      ui->adBlocks->layout()->addWidget(adBlockItem);
      connect(adBlockItem, SIGNAL(remove(AdBlockItem*)), this, SLOT(removeAdBlock(AdBlockItem*)));
    }
}

ChannelConfigView::~ChannelConfigView()
{
    delete ui;
}


void ChannelConfigView::removeAdBlock(AdBlockItem* item)
{
  int index = ui->adBlocks->layout()->indexOf(item);
  m_channelConfig.m_adBlocks.removeAt(index);
  ui->adBlocks->layout()->removeWidget(item);
  delete item;
}

void ChannelConfigView::addAdBlock()
{
  m_channelConfig.m_adBlocks.push_back(CAdBlock(QDateTime().currentDateTime(), QTime(0,0,30), "new"));
  AdBlockItem* item = new AdBlockItem(&m_channelConfig.m_adBlocks.last(), this);
  ui->adBlocks->layout()->addWidget(item);
  connect(item, SIGNAL(remove(AdBlockItem*)), this, SLOT(removeAdBlock(AdBlockItem*)));
}

void ChannelConfigView::collectData()
{
  m_channelConfig.m_splicerAddress = ui->splicerAddress->text();
  m_channelConfig.m_channelName = ui->channelName->text();
  m_channelConfig.m_splicerName = ui->splicerName->text();
  int i;
  bool ok;
  i = ui->chassis->text().toInt(&ok);
  if (ok)
  {
    m_channelConfig.m_chassis = i;
  }
  i = ui->card->text().toInt(&ok);
  if (ok)
  {
    m_channelConfig.m_card = i;
  }
  i = ui->port->text().toInt(&ok);
  if (ok)
  {
    m_channelConfig.m_port = i;
  }
  i = ui->timeBeforeSplice->text().toInt(&ok);
  if (ok)
  {
    m_channelConfig.m_secondsBeforeSpliceRequest = i;
  }
  m_channelConfig.m_processScte35 = (ui->processScte35->checkState() == Qt::Checked);
}
