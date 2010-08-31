#ifndef CHANNELCONFIGITEM_H
#define CHANNELCONFIGITEM_H

#include <QWidget>
#include "channelconfig.h"
#include "adchannel.h"

namespace Ui {
  class ChannelConfigItem;
}
class CChannelConfig;
class CAdChannel;

class ChannelConfigItem : public QWidget
{
  Q_OBJECT

public:
  explicit ChannelConfigItem(CChannelConfig channelConfig, QWidget *parent = 0);
  explicit ChannelConfigItem(CAdChannel* adChannel, QWidget *parent = 0);
  ~ChannelConfigItem();

  const CChannelConfig& getChannelConfig() const { return m_channelConfig; }
  //void SetAdChannel(CAdChannel* adChannel);


public slots:
  void connectToggle();
  void showLog();
  void edit();
  void remove();
  void stateChanged(CAdState::AdState_t newState);
  void connectSplicer();
  void disconnectSplicer();

signals:
  void removed(ChannelConfigItem* item);

private:
  void updateUi();


private:
  Ui::ChannelConfigItem *ui;
  CChannelConfig m_channelConfig;
  CAdChannel* m_adChannel;
  QPalette m_normalPalette;
  QPalette m_greenPalette;
};

#endif // CHANNELCONFIGITEM_H
