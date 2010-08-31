#include "sessionsmodel.h"
#include <QDebug>
#include <QTextStream>
#include <QBrush>

CSessionsModel::CSessionsModel(const CSpliceSessions* spliceSessions, QObject *parent):
    QAbstractTableModel(parent),
    m_spliceSessions(spliceSessions)
{
  connect(m_spliceSessions, SIGNAL(sessionChanged(int)), this, SLOT(sessionChanged(int)));
  connect(m_spliceSessions, SIGNAL(sessionAdded(int)), this, SLOT(sessionAdded(int)));
}

int CSessionsModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_spliceSessions->count();
}

int CSessionsModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 6;
}

QVariant CSessionsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_spliceSessions->count() || index.row() < 0)
    return QVariant();


  if (role == Qt::DisplayRole)
  {
    CSpliceSession* spliceSession = m_spliceSessions->at(index.row());
    if(!spliceSession)
      return QVariant();

    if (index.column() == 0)
      return spliceSession->getSessionId();
    else if (index.column() == 1)
    {
      if (spliceSession->getPriorSessionId() != 0xFFFFFFFF)
        return spliceSession->getPriorSessionId();
      else return "None";
    }
    else if (index.column() == 2)
      return spliceSession->getStartTime().toString(Qt::ISODate);
    else if (index.column() == 3)
      return Helpers::ticksToString(spliceSession->getExpectedDuration());
    else if (index.column() == 4)
      return Helpers::ticksToString(spliceSession->getDuration());
    else if (index.column() == 5)
      return CSpliceSession::toString(spliceSession->getSessionState());
  }
  if (role == Qt::BackgroundRole)
  {
    CSpliceSession* spliceSession = m_spliceSessions->at(index.row());
    if(!spliceSession)
      return QVariant();
    switch(spliceSession->getSessionState())
    {
    case CSpliceSession::Finished:
      return QBrush(Qt::green);
    case CSpliceSession::Failed:
      return QBrush(Qt::red);
    default:
      return QVariant();
    }
  }
  return QVariant();
}
QVariant CSessionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Session ID");
    case 1:
      return tr("Prior Session ID");
    case 2:
      return tr("Start Time");
    case 3:
      return tr("Expected Duration");
    case 4:
      return tr("Duration");
    case 5:
      return tr("State");
    default:
      return QVariant();
    }
  }
  return QVariant();
}
Qt::ItemFlags CSessionsModel::flags(const QModelIndex &index) const
{
  Q_UNUSED(index);
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool CSessionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  Q_UNUSED(index);
  Q_UNUSED(value);
  Q_UNUSED(role);
  qWarning() << "Setting data on a SessionsModel is not supported";
  return false;
}

bool CSessionsModel::insertRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  Q_UNUSED(position);
  Q_UNUSED(rows);
  qWarning() << "Inserting rows in a SessionsModel is not supported";
  return false;
}

bool CSessionsModel::removeRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  Q_UNUSED(position);
  Q_UNUSED(rows);
  qWarning() << "removing rows from a SessionsModel is not supported";
  return false;
}

void CSessionsModel::sessionChanged(int pos)
{
  dataChanged(index(pos,0), index(pos,5));
}

void CSessionsModel::sessionAdded(int pos)
{
  beginInsertRows(QModelIndex(), pos, pos);
  endInsertRows();
}

