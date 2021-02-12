#include "generalsubsrciberwidget.h"

#include <QPushButton>

GeneralSubsrciberWidget::GeneralSubsrciberWidget(QWidget *parent) : QWidget(parent)
{
    QPushButton button("My text", this);
    button.setToolTip("A tooltip");
    button.show();
}
