#ifndef ADBLOCKITEM_H
#define ADBLOCKITEM_H

#include <QWidget>

namespace Ui {
  class adblockitem;
}
class CAdBlock;

class AdBlockItem : public QWidget
{
  Q_OBJECT

public:
  explicit AdBlockItem(CAdBlock* adBlock, QWidget *parent = 0);
  ~AdBlockItem();

public slots:
  void remove();
  void edit();
  void collectData();
signals:
  void remove(AdBlockItem* item);
private:
  void update();

private:
  Ui::adblockitem *ui;
  CAdBlock* m_adBlock;
};

#endif // ADBLOCKITEM_H
