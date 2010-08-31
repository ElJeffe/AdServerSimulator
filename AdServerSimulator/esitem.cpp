#include "esitem.h"
#include "ui_esitem.h"
#include "channelconfig.h"
#include "descriptoritem.h"
#include <QString>


EsItem::EsItem(CSpliceElementaryStream* spliceStream, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EsItem),
    m_es(spliceStream)
{
    ui->setupUi(this);
    ui->pid->setText(QString::number(m_es->m_pid));
    ui->streamType->setText(QString::number(m_es->m_streamType));
    ui->descriptors->setLayout(new QVBoxLayout(ui->descriptors));
    for (QList<CDescriptor>::iterator it = m_es->m_descriptors.begin(); it != m_es->m_descriptors.end(); ++it)
    {
      DescriptorItem* item = new DescriptorItem(&(*it), this);
      ui->descriptors->layout()->addWidget(item);
      connect(item, SIGNAL(remove(DescriptorItem*)), this, SLOT(removeDescriptor(DescriptorItem*)));
      connect(this, SIGNAL(reqCollectData()), item, SLOT(collectData()));
    }
}

EsItem::~EsItem()
{
    delete ui;
}

void EsItem::remove()
{
  remove(this);
}

void EsItem::removeDescriptor(DescriptorItem* item)
{
  int index = ui->descriptors->layout()->indexOf(item);
  m_es->m_descriptors.removeAt(index);
  ui->descriptors->layout()->removeWidget(item);
  delete item;
}
void EsItem::addDescriptor()
{
  m_es->m_descriptors.push_back(CDescriptor());
  DescriptorItem* item = new DescriptorItem(&m_es->m_descriptors.last(), this);
  ui->descriptors->layout()->addWidget(item);
  connect(item, SIGNAL(remove(DescriptorItem*)), this, SLOT(removeDescriptor(DescriptorItem*)));
  connect(this, SIGNAL(reqCollectData()), item, SLOT(collectData()));
}

void EsItem::collectData()
{
  reqCollectData();
  quint16 u16;
  bool ok;
  u16 = ui->pid->text().toUShort(&ok);
  if (ok)
  {
    m_es->m_pid = u16;
  }
  u16 = ui->streamType->text().toUShort(&ok);
  if (ok)
  {
    m_es->m_streamType = u16;
  }
}
