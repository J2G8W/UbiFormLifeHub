#include "window.h"
#include <QApplication>
#include <QLabel>
#include <iostream>

Window::Window(QWidget *parent) : QWidget(parent)
{
    setFixedSize(300,300);

    connect(this, SIGNAL(createNewLabel(int)), this, SLOT(slotCreateNewLabel(int)));
    connect(this, SIGNAL(updateText(QString,int)), this, SLOT(slotChangeText(QString,int)));
    connect(this, SIGNAL(updateColour(QColor,int)), this, SLOT(slotChangeColour(QColor,int)));
}

void Window::slotChangeText(QString text, int labelNum){

    QLabel* label = labels.at(labelNum);
    label->setText(text);
}
void Window::slotCreateNewLabel(int numLines){
    std::cout << "Label created" << std::endl;
    QLabel* newLabel = new QLabel(this);
    newLabel->setGeometry(10,y, 200, 20 * numLines);
    newLabel->show();
    y += 20 * numLines;
    labels.push_back(newLabel);
}

void Window::slotChangeColour(QColor colour, int labelNum){
    QLabel* label = labels.at(labelNum);
    QPalette p;
    p.setColor(QPalette::WindowText,colour);
    label->setPalette(p);
}

void updateText(QString text, int labelNum);
void createNewLabel();
