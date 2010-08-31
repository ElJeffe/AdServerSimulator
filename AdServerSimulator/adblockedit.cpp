#include "adblockedit.h"
#include "ui_adblockedit.h"
#include "adconfigitem.h"
#include "channelconfig.h"

AdBlockEdit::AdBlockEdit(CAdBlock* adBlock, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdBlockEdit),
    m_adBlock(adBlock)
{
    ui->setupUi(this);
    ui->name->setText(m_adBlock->m_name);
    ui->startTime->setDateTime(m_adBlock->m_startTime);
    ui->recurringTime->setTime(m_adBlock->m_recurringTime);

    ui->ads->setLayout(new QHBoxLayout(ui->ads));
    for (QList<CAdConfig>::iterator it = m_adBlock->m_adConfigs.begin(); it != m_adBlock->m_adConfigs.end(); ++it)
    {
      AdConfigItem* adConfigItem = new AdConfigItem(&(*it), this);
      ui->ads->layout()->addWidget(adConfigItem);
      connect(adConfigItem, SIGNAL(remove(AdConfigItem*)), this, SLOT(removeAdConfig(AdConfigItem*)));
      connect(this, SIGNAL(reqCollectData()), adConfigItem, SLOT(collectData()));
    }
}

AdBlockEdit::~AdBlockEdit()
{
    delete ui;
}

void AdBlockEdit::removeAdConfig(AdConfigItem* item)
{
  int index = ui->ads->layout()->indexOf(item);
  m_adBlock->m_adConfigs.removeAt(index);

  ui->ads->layout()->removeWidget(item);
  delete item;
}

void AdBlockEdit::addAdConfig()
{
  m_adBlock->m_adConfigs.push_back(CAdConfig());
  AdConfigItem* item = new AdConfigItem(&m_adBlock->m_adConfigs.last(), this);
  ui->ads->layout()->addWidget(item);
  connect(item, SIGNAL(remove(AdConfigItem*)), this, SLOT(removeAdConfig(AdConfigItem*)));
  connect(this, SIGNAL(reqCollectData()), item, SLOT(collectData()));
}


void AdBlockEdit::collectData()
{
  reqCollectData();
  m_adBlock->m_name = ui->name->text();
  m_adBlock->m_startTime = ui->startTime->dateTime();
  m_adBlock->m_recurringTime = ui->recurringTime->time();
}
