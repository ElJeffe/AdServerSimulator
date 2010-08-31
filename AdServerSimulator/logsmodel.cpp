#include "logsmodel.h"
#include "channellogger.h"
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QPixmap>

/**************************************************************************
 * LogFilterModel
 **************************************************************************/

CLogFilterModel::CLogFilterModel(QObject *parent):
    QSortFilterProxyModel(parent),
    m_showInfo(true),
    m_showWarnings(true),
    m_showMessages(true),
    m_showErrors(true)
{
}

void CLogFilterModel::setShowInfo(bool showInfo)
{
  m_showInfo = showInfo;
  invalidateFilter();
}

void CLogFilterModel::setShowWarnings(bool showWarnings)
{
  m_showWarnings = showWarnings;
  invalidateFilter();
}

void CLogFilterModel::setShowMessages(bool showMessages)
{
  m_showMessages = showMessages;
  invalidateFilter();
}

void CLogFilterModel::setShowErrors(bool showErrors)
{
  m_showErrors = showErrors;
  invalidateFilter();
}

bool CLogFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
  QModelIndex typeIndex = sourceModel()->index(sourceRow, 0, sourceParent);
  QVariant logLevel = sourceModel()->data(typeIndex);
  if(logLevel.toString() == "Info")
    return m_showInfo;
  else if(logLevel.toString() ==  "Warning")
    return m_showWarnings;
  else if(logLevel.toString() ==  "Error")
    return m_showErrors;
  else if(logLevel.toString() == "MessageRcv")
    return m_showMessages;
  else if(logLevel.toString() == "MessageSend")
    return m_showMessages;
  qWarning() << "Unknown message type: " << logLevel.toString();
  return false;
}



/**************************************************************************
 * CLogsModel
 **************************************************************************/

CLogsModel::CLogsModel(const CChannelLogger* channelLogger, QObject* parent):
    QAbstractTableModel(parent),
    m_channelLogger(channelLogger)
{
  connect(m_channelLogger, SIGNAL(logAdded(int)), this, SLOT(logAdded(int)));
}

int CLogsModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_channelLogger->count();
}

int CLogsModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 4;
}

QVariant CLogsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_channelLogger->count() || index.row() < 0)
    return QVariant();

  const CLogLine* logLine = m_channelLogger->at(index.row());

  // normal data
  if (role == Qt::DisplayRole) {
    if(!logLine)
      return QVariant();

    if (index.column() == 0)
      return CLogLine::toString(logLine->getLogLevel());
    else if (index.column() == 2)
      return logLine->getTime().toString();
    else if (index.column() == 3)
    {
      int openPos = logLine->getLog().indexOf('{');
      if (openPos == -1)
        return logLine->getLog();
      else
        return logLine->getLog().left(openPos).trimmed();
    }
  }

  // Tooltips
  else if (role == Qt::ToolTipRole)
  {
    if (index.column() == 3)
    {
      if (logLine->getLog().contains('{'))
      {
        return prettyMessage(logLine->getLog(), "");
      }
    }
  }

  // icons
  else if (role == Qt::DecorationRole) {
    if (index.column() == 1)
    {
      if (logLine->getLogLevel() == CLogLine::Info)
        return QPixmap(":/images/info.png");
      if (logLine->getLogLevel() == CLogLine::Warning)
        return QPixmap(":/images/warning.png");
      if (logLine->getLogLevel() == CLogLine::Error)
        return QPixmap(":/images/error.png");
      if (logLine->getLogLevel() == CLogLine::MessageRcv)
        return QPixmap(":/images/left.png");
      if (logLine->getLogLevel() == CLogLine::MessageSend)
        return QPixmap(":/images/right.png");
      return QVariant();
    }
  }

  return QVariant();
}
QVariant CLogsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Type");
    case 1:
      return tr("");
    case 2:
      return tr("Time");
    case 3:
      return tr("Log");
    default:
      return QVariant();
    }
  }
  return QVariant();
}
Qt::ItemFlags CLogsModel::flags(const QModelIndex &index) const
{
  Q_UNUSED(index);
  return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

bool CLogsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  Q_UNUSED(index);
  Q_UNUSED(value);
  Q_UNUSED(role);
  qWarning() << "Setting data on a LogModel is not supported";
  return false;
}

bool CLogsModel::insertRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  Q_UNUSED(position);
  Q_UNUSED(rows);
  qWarning() << "Inserting rows in a CLogsModel is not supported";
  return false;
}

bool CLogsModel::removeRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  Q_UNUSED(position);
  Q_UNUSED(rows);
  qWarning() << "removing rows from a CLogsModel is not supported";
  return false;
}

void CLogsModel::logAdded(int pos)
{
  beginInsertRows(QModelIndex(), pos, pos);
  endInsertRows();
}

QString CLogsModel::prettyMessage(const QString& log, QString indent) const
{
  QString res = "";
  QTextStream str(&res);
  int openPos = log.indexOf('{');
  int closePos = log.indexOf('}');
  if (openPos == -1 && closePos == -1)
  {
    str << log;
    return res;
  }
  if ((openPos != -1) && (openPos < closePos))
  {
    QStringList list = log.left(openPos).split(';');
    for (QStringList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
      QString prop = (*it).trimmed();
      if (prop.size() > 0)
        str << indent << prop << "\n";
    }

    str << prettyMessage(log.mid(openPos + 1), indent.append("    "));
  }
  else if (closePos != -1)
  {
    QStringList list = log.left(closePos).split(';');
    for (QStringList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
      QString prop = (*it).trimmed();
      if (prop.size() > 0)
        str << indent << prop << "\n";
    }
    str << prettyMessage(log.mid(closePos + 1), indent.mid(4));
  }
  return res;
}
