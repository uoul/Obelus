#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "eventwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateEventSelection();

    void onEventSelectionChanged();

    void onEventTeamNameChanged();

    void updateEventOverview();

    void updateEventTeamView();

    void on_actionRules_triggered();

    void on_actionExit_triggered();

    void on_pushButton_eventCreate_clicked();

    void on_pushButton_EventDel_clicked();

private:
    Ui::MainWindow *ui;
    EventWindow* eventWindow;
};
#endif // MAINWINDOW_H
