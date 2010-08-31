#include "channellogview.h"
#include "ui_channellogview.h"
#include "sessionsmodel.h"
#include "logsmodel.h"
#include "adchannel.h"
#include <QFileDialog>

ChannelLogView::ChannelLogView(const CAdChannel* adChannel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelLogView),
    m_adChannel(adChannel)
{
  ui->setupUi(this);
  setWindowTitle(QString("Channel Log - %1 on %2").arg(m_adChannel->getChannelConfig().getChannelName(), m_adChannel->getChannelConfig().getSplicerAddress()));
  // setup splice sessions
  CSessionsModel* sessionsModel = new CSessionsModel(m_adChannel->getSpliceSessions(), this);
  ui->sessionsView->setModel(sessionsModel);
  ui->sessionsView->resizeColumnsToContents();
  ui->sessionsView->horizontalHeader()->setStretchLastSection(true);
  connect(sessionsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), ui->sessionsView, SLOT(resizeColumnsToContents()));
  // setup logger
  CLogsModel* logsModel = new CLogsModel(m_adChannel->getChannelLogger(), this);
  m_logFilterModel = new CLogFilterModel(this);
  connect(ui->cbInfo, SIGNAL(clicked(bool)), m_logFilterModel, SLOT(setShowInfo(bool)));
  connect(ui->cbWarnings, SIGNAL(clicked(bool)), m_logFilterModel, SLOT(setShowWarnings(bool)));
  connect(ui->cbErrors, SIGNAL(clicked(bool)), m_logFilterModel, SLOT(setShowErrors(bool)));
  connect(ui->cbMessages, SIGNAL(clicked(bool)), m_logFilterModel, SLOT(setShowMessages(bool)));
  m_logFilterModel->setSourceModel(logsModel);
  ui->logsView->setModel(m_logFilterModel);
  ui->logsView->hideColumn(0);
  ui->logsView->resizeColumnsToContents();
  ui->logsView->horizontalHeader()->setStretchLastSection(true);
}

ChannelLogView::~ChannelLogView()
{
  delete ui;
}

void ChannelLogView::clearLog()
{
  m_adChannel->getChannelLogger()->clear();
  m_logFilterModel->invalidate();
}

void ChannelLogView::saveLog()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Settings"),QDir::currentPath(), tr("XML files (*.xml);;All files (*.*"));
  if (fileName.isEmpty())
  {
    return;
  }
  m_adChannel->saveLog(fileName);
}


