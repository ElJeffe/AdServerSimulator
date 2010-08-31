#ifndef ADSERVERSIMULATOR_H
#define ADSERVERSIMULATOR_H

#include <QMainWindow>
#include "splicestates.h"
#include "channelconfig.h"

namespace Ui {
    class AdServerSimulator;
}
class ChannelConfigItem;

class AdServerSimulator : public QMainWindow
{
  Q_OBJECT

public:
  explicit AdServerSimulator(QWidget *parent = 0);
  ~AdServerSimulator();

public slots:
  void openFile();
  void addFromFile();
  void saveFile();
  void addChannel();
  void connectAll();
  void disconnectAll();
  void openLog();
private slots:
  void removeChannel(ChannelConfigItem* item);

private:
  bool readChannelSettings(QString fileName);
  bool readChannelConfigs(QXmlStreamReader& reader);
  void addReadChannelConfigItems();

private:
  Ui::AdServerSimulator *ui;
  QString m_currentDir;
  QList<CChannelConfig> m_readChannelConfigs;
  QList<ChannelConfigItem*> m_channelConfigItems;
};

#endif // ADSERVERSIMULATOR_H
