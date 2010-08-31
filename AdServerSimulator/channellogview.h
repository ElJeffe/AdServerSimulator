#ifndef CHANNELLOGVIEW_H
#define CHANNELLOGVIEW_H

#include <QDialog>

class CAdChannel;
class CLogFilterModel;
namespace Ui {
  class ChannelLogView;
}

class ChannelLogView : public QDialog
{
  Q_OBJECT

public:
  explicit ChannelLogView(const CAdChannel* adChannel, QWidget *parent = 0);
  ~ChannelLogView();
public slots:
  void clearLog();
  void saveLog();

private:
  Ui::ChannelLogView *ui;
  const CAdChannel* m_adChannel;
  CLogFilterModel* m_logFilterModel;
};

#endif // CHANNELLOGVIEW_H
