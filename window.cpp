#include "window.h"
#include <QApplication>
#include <QLabel>
#include <iostream>

Window::Window(QWidget *parent) : QWidget(parent)
{
    setFixedSize(300,300);

    connect(this, SIGNAL(createNewLabel()), this, SLOT(slotCreateNewLabel()));
    connect(this, SIGNAL(updateText(QString,int)), this, SLOT(slotChangeText(QString,int)));
}

void Window::slotChangeText(QString text, int labelNum){

    QLabel* label = labels.at(labelNum);
    label->setText(text);
}
void Window::slotCreateNewLabel(){
    std::cout << "Label created" << std::endl;
    QLabel* newLabel = new QLabel(this);
    newLabel->setText("HELLO");

    newLabel->setGeometry(10,y, 200, 20);
    newLabel->show();
    y += 20;
    labels.push_back(newLabel);
}

void updateText(QString text, int labelNum);
void createNewLabel();
