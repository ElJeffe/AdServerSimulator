#ifndef ADCONFIGITEM_H
#define ADCONFIGITEM_H

#include <QGroupBox>

namespace Ui {
  class AdConfigItem;
}
class CAdConfig;
class EsItem;

class AdConfigItem : public QFrame
{
  Q_OBJECT

public:
  explicit AdConfigItem(CAdConfig* adConfig, QWidget *parent = 0);
  ~AdConfigItem();
  CAdConfig* getAdConfig() { return m_adConfig; }
public slots:
  void remove();
  void removeEs(EsItem* item);
  void addEs();
  void collectData();
signals:
  void remove(AdConfigItem* item);
  void reqCollectData();

private:
  Ui::AdConfigItem *ui;
  CAdConfig* m_adConfig;
};

#endif // ADCONFIGITEM_H
