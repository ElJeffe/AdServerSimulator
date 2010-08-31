#ifndef LOGSMODEL_H
#define LOGSMODEL_H

#include <QAbstractTableModel>
#include<QSortFilterProxyModel>

class CChannelLogger;

class CLogFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CLogFilterModel(QObject *parent = 0);

  public slots:
    void setShowInfo(bool showInfo);
    void setShowWarnings(bool showWarnings);
    void setShowMessages(bool showMessages);
    void setShowErrors(bool showErrors);

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
private:
    bool m_showInfo;
    bool m_showWarnings;
    bool m_showMessages;
    bool m_showErrors;
};

class CLogsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
  CLogsModel(const CChannelLogger* channelLogger, QObject *parent=0);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
  bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
private slots:
  void logAdded(int pos);

private:
  const CChannelLogger* m_channelLogger;
  QString prettyMessage(const QString& log, QString indent) const;
};

#endif // LOGSMODEL_H
