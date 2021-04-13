#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include <QWidget>
#include "global.h"

namespace Ui {
class EventWindow;
}

class EventWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EventWindow(QWidget *parent = nullptr);
    ~EventWindow();

signals:
    void eventCreated();

private slots:
    void on_commandLinkButton_create_clicked();

private:
    Ui::EventWindow *ui;
    void initUi();
};

#endif // EVENTWINDOW_H
