#ifndef SESSIONSMODEL_H
#define SESSIONSMODEL_H

#include <QAbstractTableModel>
#include "splicesessions.h"

class CSessionsModel: public QAbstractTableModel
{
  Q_OBJECT

public:
  CSessionsModel(const CSpliceSessions* spliceSessions, QObject *parent=0);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
  bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
private slots:
  void sessionChanged(int pos);
  void sessionAdded(int pos);

private:
  const CSpliceSessions* m_spliceSessions;
};

#endif // SESSIONSMODEL_H
