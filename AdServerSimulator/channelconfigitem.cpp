#include "channelconfigitem.h"
#include "ui_channelconfigitem.h"
#include "channelconfig.h"
#include "adchannel.h"
#include "channellogview.h"
#include "channelconfigview.h"
#include <QDebug>

// temp
#include "splicesessions.h"

ChannelConfigItem::ChannelConfigItem(CChannelConfig channelConfig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelConfigItem),
    m_channelConfig(channelConfig),
    m_adChannel(0)
{
  ui->setupUi(this);
  updateUi();
  m_normalPalette = ui->state->palette();
  m_greenPalette = ui->state->palette();
  m_greenPalette.setColor(QPalette::Base, Qt::green);
  m_adChannel = new CAdChannel(this);
  connect(m_adChannel, SIGNAL(stateChanged(CAdState::AdState_t)), this, SLOT(stateChanged(CAdState::AdState_t)));
}

ChannelConfigItem::ChannelConfigItem(CAdChannel* adChannel, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ChannelConfigItem),
    m_adChannel(adChannel)
{
  ui->setupUi(this);
  m_normalPalette = ui->state->palette();
  m_greenPalette = ui->state->palette();
  m_greenPalette.setColor(QPalette::Base, Qt::green);
  ui->logBtn->setEnabled(true);
  ui->connectBtn->setEnabled(false);

  m_adChannel->setParent(this);
  m_channelConfig = m_adChannel->getChannelConfig();
  connect(m_adChannel, SIGNAL(stateChanged(CAdState::AdState_t)), this, SLOT(stateChanged(CAdState::AdState_t)));
  updateUi();
}

//void ChannelConfigItem::SetAdChannel(CAdChannel* adChannel)
//{
//  m_adChannel = adChannel;
//  m_adChannel->setParent(this);
//  m_channelConfig = m_adChannel->getChannelConfig();
//  connect(m_adChannel, SIGNAL(stateChanged(CAdState::AdState_t)), this, SLOT(stateChanged(CAdState::AdState_t)));
//  ui->logBtn->setEnabled(true);
//}


ChannelConfigItem::~ChannelConfigItem()
{
  delete ui;
}

void ChannelConfigItem::updateUi()
{
  ui->channelText->setText(QString("%1 on %2").arg(m_channelConfig.getChannelName(), m_channelConfig.getSplicerAddress()));
}

void ChannelConfigItem::connectToggle()
{
  if (m_adChannel->state() == CAdState::StateNone)
  {
    connectSplicer();
  }
  else
  {
    disconnectSplicer();
  }
}

void ChannelConfigItem::connectSplicer()
{
  if (m_adChannel->state() == CAdState::StateNone)
  {
    m_adChannel->setChannelConfig(m_channelConfig);
    m_adChannel->connectSplicer();
    ui->logBtn->setEnabled(true);
  }
}

void ChannelConfigItem::disconnectSplicer()
{
  if (m_adChannel->state() != CAdState::StateNone)
  {
    m_adChannel->disconnectSplicer();
  }
}


void ChannelConfigItem::showLog()
{
  if (!m_adChannel)
  {
    return;
  }
  ChannelLogView channelLogView(m_adChannel, this);
  channelLogView.exec();
}

void ChannelConfigItem::edit()
{
  ChannelConfigView channelConfigView(m_channelConfig, this);
  if (channelConfigView.exec() == QDialog::Accepted)
  {
    channelConfigView.collectData();
    m_channelConfig = channelConfigView.getChannelConfig();
    updateUi();
  }
}

void ChannelConfigItem::remove()
{
  qDebug() << "removed";
  removed(this);
}

void ChannelConfigItem::stateChanged(CAdState::AdState_t newState)
{
  if (newState == CAdState::StateNone)
  {
    ui->deleteBtn->setEnabled(true);
    ui->editBtn->setEnabled(true);
    ui->connectBtn->setIcon(QIcon(":/images/connect.png"));
    ui->connectBtn->setToolTip("Connect");
  }
  else
  {
    ui->deleteBtn->setEnabled(false);
    ui->editBtn->setEnabled(false);
    ui->connectBtn->setIcon(QIcon(":/images/disconnect.png"));
    ui->connectBtn->setToolTip("Disconnect");
  }
  switch (newState)
  {
  case CAdState::StateNone:
    ui->state->setText("Disconnected");
    ui->state->setPalette(m_normalPalette);
    break;
  case CAdState::StateInit:
    ui->state->setText("Initializing");
    ui->state->setPalette(m_normalPalette);
    break;
  case CAdState::StateOnMain:
    ui->state->setText("On main");
    ui->state->setPalette(m_normalPalette);
    break;
  case CAdState::StateOnInsertion:
    ui->state->setText("On insertion");
    ui->state->setPalette(m_greenPalette);
    break;
  default:
    qWarning() << "unknown state " << CAdState::toString(newState);
    break;
  }
}
