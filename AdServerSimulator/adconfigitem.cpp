#include "adconfigitem.h"
#include "ui_adconfigitem.h"
#include "channelconfig.h"
#include "helpers.h"
#include "esitem.h"

AdConfigItem::AdConfigItem(CAdConfig* adConfig, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AdConfigItem),
    m_adConfig(adConfig)
{
  ui->setupUi(this);
  ui->serviceId->setText(QString::number(m_adConfig->m_serviceId));
  ui->pcrPid->setText(QString::number(m_adConfig->m_pcrPid));
  ui->duration->setTime(Helpers::ticksToTime(m_adConfig->m_duration));
  ui->postBlack->setText(QString::number(m_adConfig->m_postBlack));
  ui->accessType->setText(QString::number(m_adConfig->m_accessType));
  ui->overridePlaying->setChecked(m_adConfig->m_overridePlaying);
  ui->returnToPrior->setChecked(m_adConfig->m_returnToPriorChannel);
  ui->es->setLayout(new QVBoxLayout(ui->es));
  for (QList<CSpliceElementaryStream>::iterator it = m_adConfig->m_spliceElementaryStreams.begin(); it != m_adConfig->m_spliceElementaryStreams.end(); ++it)
  {
    EsItem* esItem = new EsItem(&(*it), this);
    ui->es->layout()->addWidget(esItem);
    connect(esItem, SIGNAL(remove(EsItem*)), this, SLOT(removeEs(EsItem*)));
    connect(this, SIGNAL(reqCollectData()), esItem, SLOT(collectData()));
  }
}

AdConfigItem::~AdConfigItem()
{
  delete ui;
}

void AdConfigItem::remove()
{
  remove(this);
}

void AdConfigItem::removeEs(EsItem* item)
{
  int index = ui->es->layout()->indexOf(item);
  m_adConfig->m_spliceElementaryStreams.removeAt(index);

  ui->es->layout()->removeWidget(item);
  delete item;
}

void AdConfigItem::addEs()
{
  m_adConfig->m_spliceElementaryStreams.push_back(CSpliceElementaryStream());
  EsItem* item = new EsItem(&m_adConfig->m_spliceElementaryStreams.last(), this);
  ui->es->layout()->addWidget(item);
  connect(item, SIGNAL(remove(EsItem*)), this, SLOT(removeEs(EsItem*)));
  connect(this, SIGNAL(reqCollectData()), item, SLOT(collectData()));
}

void AdConfigItem::collectData()
{
  reqCollectData();
  quint8 u8;
  quint16 u16;
  quint32 u32;
  bool ok;
  u16 = ui->serviceId->text().toUShort(&ok);
  if (ok)
  {
    m_adConfig->m_serviceId = u16;
  }
  u16 = ui->pcrPid->text().toUShort(&ok);
  if (ok)
  {
    m_adConfig->m_pcrPid = u16;
  }
  m_adConfig->setDuration(ui->duration->time());
  u32 = ui->postBlack->text().toUInt(&ok);
  if (ok)
  {
    m_adConfig->m_postBlack = u32;
  }
  u8 = ui->accessType->text().toUShort(&ok);
  if (ok)
  {
    m_adConfig->m_accessType = u8;
  }
  m_adConfig->m_overridePlaying = (ui->overridePlaying->checkState() == Qt::Checked);
  m_adConfig->m_returnToPriorChannel = (ui->returnToPrior->checkState() == Qt::Checked);
}
