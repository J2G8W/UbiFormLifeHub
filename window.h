#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QWidget>

class QLabel;
class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
private:
    std::vector<QLabel*> labels;
    int y = 10;

signals:
    void updateText(QString text, int labelNum);
    void createNewLabel();
public slots:
    void slotChangeText(QString text, int LabelNum);
    void slotCreateNewLabel();

};

#endif // WINDOW_H
