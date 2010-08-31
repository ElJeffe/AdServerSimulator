#include "descriptoritem.h"
#include "ui_descriptoritem.h"
#include "channelconfig.h"
#include <QDebug>

DescriptorItem::DescriptorItem(CDescriptor* descriptor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptorItem),
    m_descriptor(descriptor)
{
    ui->setupUi(this);
    ui->tag->setText(QString::number(m_descriptor->m_tag));
    ui->data->setText(m_descriptor->dataToString());
}

DescriptorItem::~DescriptorItem()
{
    delete ui;
}

void DescriptorItem::remove()
{
  remove(this);
}

void DescriptorItem::collectData()
{
  bool ok;
  quint8 u8 = ui->tag->text().toUShort(&ok);
  if (ok)
  {
    m_descriptor->m_tag = u8;
  }
  m_descriptor->dataFromString(ui->data->text());
}

