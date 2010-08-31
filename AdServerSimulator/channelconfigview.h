#ifndef CHANNELCONFIGVIEW_H
#define CHANNELCONFIGVIEW_H

#include <QDialog>
#include "channelconfig.h"

class AdBlockItem;

namespace Ui {
  class ChannelConfigView;
}

class ChannelConfigView : public QDialog
{
  Q_OBJECT

public:
  explicit ChannelConfigView(const CChannelConfig& channelConfig, QWidget *parent = 0);
  ~ChannelConfigView();
  const CChannelConfig& getChannelConfig() { return m_channelConfig; }
public slots:
  void collectData();

public slots:
  void removeAdBlock(AdBlockItem* adBlock);
  void addAdBlock();


private:
  Ui::ChannelConfigView *ui;
  CChannelConfig m_channelConfig;
};

#endif // CHANNELCONFIGVIEW_H
