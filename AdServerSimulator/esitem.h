#ifndef ESITEM_H
#define ESITEM_H

#include <QWidget>

class CSpliceElementaryStream;
class DescriptorItem;
namespace Ui {
  class EsItem;
}


class EsItem : public QWidget
{
  Q_OBJECT

public:
  explicit EsItem(CSpliceElementaryStream* spliceStream, QWidget *parent = 0);
  ~EsItem();
public slots:
  void remove();
  void removeDescriptor(DescriptorItem* item);
  void addDescriptor();
  void collectData();

signals:
  void remove(EsItem* item);
  void reqCollectData();
private:
  Ui::EsItem *ui;
  CSpliceElementaryStream* m_es;

};

#endif // ESITEM_H
