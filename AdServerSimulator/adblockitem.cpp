#include "adblockitem.h"
#include "ui_adblockitem.h"
#include "adblockedit.h"
#include "channelconfig.h"


AdBlockItem::AdBlockItem(CAdBlock* adBlock, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adblockitem),
    m_adBlock(adBlock)
{
  ui->setupUi(this);
  update();
}

AdBlockItem::~AdBlockItem()
{
  delete ui;
}

void AdBlockItem::update()
{
  ui->name->setText(m_adBlock->m_name);
}

void AdBlockItem::remove()
{
  remove(this);
}

void AdBlockItem::edit()
{
  AdBlockEdit adBlockEdit(m_adBlock, this);
  if (adBlockEdit.exec() == QDialog::Accepted)
  {
    adBlockEdit.collectData();
    ui->name->setText(m_adBlock->m_name);
  }
}

void AdBlockItem::collectData()
{
  m_adBlock->m_name = ui->name->text();
}

