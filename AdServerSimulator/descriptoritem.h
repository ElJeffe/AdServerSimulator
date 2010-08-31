#ifndef DESCRIPTORITEM_H
#define DESCRIPTORITEM_H

#include <QWidget>

namespace Ui {
    class DescriptorItem;
}
class CDescriptor;

class DescriptorItem : public QWidget
{
    Q_OBJECT

public:
    explicit DescriptorItem(CDescriptor* descriptor, QWidget *parent = 0);
    ~DescriptorItem();
  public slots:
    void remove();
    void collectData();
signals:
    void remove(DescriptorItem* item);

private:
    Ui::DescriptorItem *ui;
    CDescriptor* m_descriptor;
};

#endif // DESCRIPTORITEM_H
