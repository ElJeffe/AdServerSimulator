#ifndef ADBLOCKEDIT_H
#define ADBLOCKEDIT_H

#include <QDialog>

namespace Ui {
  class AdBlockEdit;
}
class AdConfigItem;
class CAdBlock;

class AdBlockEdit : public QDialog
{
  Q_OBJECT

public:
  explicit AdBlockEdit(CAdBlock* adBlock, QWidget *parent = 0);
  ~AdBlockEdit();

public slots:
  void removeAdConfig(AdConfigItem* item);
  void addAdConfig();

  void collectData();
signals:
  void reqCollectData();

private:
  Ui::AdBlockEdit *ui;
  CAdBlock* m_adBlock;
};

#endif // ADBLOCKEDIT_H
