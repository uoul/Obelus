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

    void createIntermediateResult();

    void createIntermediateResultHtml();

    void initIntermediateResultDir();

    void onEventSelectionChanged();

    void onEventTeamNameChanged();

    void onEventResultChanged();

    void updateEventOverview();

    void updateEventTeamView();

    void updateMainPassageSelection();

    void updatePassageSelection();

    void updateResultTable();

    void on_actionRules_triggered();

    void on_actionExit_triggered();

    void on_pushButton_eventCreate_clicked();

    void on_pushButton_EventDel_clicked();

    void on_lineEdit_Overview_host_editingFinished();

    void on_lineEdit_Overview_exec_editingFinished();

    void on_lineEdit_Overview_location_editingFinished();

    void on_lineEdit_Overview_dateAndTime_editingFinished();

    void on_lineEdit_Overview_leader_editingFinished();

    void on_lineEdit_Overview_referee_editingFinished();

    void on_lineEdit_Overview_secretary_editingFinished();

    void on_comboBox_mainPassageSelection_currentIndexChanged(int index);

    void on_comboBox_passageSelection_currentIndexChanged(int index);

    void on_pushButton_showIntermediateResult_clicked();

private:
    Ui::MainWindow *ui;
    EventWindow* eventWindow;
};
#endif // MAINWINDOW_H
