#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup MainWindow
    ui->setupUi(this);

    // Create other windows
    eventWindow = new EventWindow;

    // set spectial actions
    eventWindow->setAttribute(Qt::WA_QuitOnClose, false);

    // start init. functions
    updateEventSelection();

    // connect signals & slots
    connect(eventWindow,SIGNAL(eventCreated()),this,SLOT(updateEventSelection()));
    connect(ui->listWidget_eventSelection,SIGNAL(itemSelectionChanged()),this,SLOT(onEventSelectionChanged()));
    connect(ui->tableWidget_teamConfig,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(onEventTeamNameChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete eventWindow;
}

void MainWindow::updateEventSelection()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Init.
    //-------------------------------------------------------------------------------------------------------------------------------
    this->ui->listWidget_eventSelection->clear();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch Events from Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    query.exec("SELECT event_name FROM events");

    while(query.next()){
        this->ui->listWidget_eventSelection->addItem(query.value(0).toString());
    }
}

void MainWindow::updateEventOverview()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch Date of the selected event
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    query.exec(tr("SELECT count_playgrounds,count_passages,count_teams,host,exec,location,date_and_time,type,class,leader,referee,secretary FROM events WHERE event_name='%1'").arg(ui->listWidget_eventSelection->currentItem()->text()));

    //-------------------------------------------------------------------------------------------------------------------------------
    // Set ui fields
    //-------------------------------------------------------------------------------------------------------------------------------
    if(query.first()){
        Global::matchClass matchClass = static_cast<Global::matchClass>(query.value(8).toInt());
        Global::matchType matchType = static_cast<Global::matchType>(query.value(7).toInt());

        ui->lineEdit_Overview_eventName->setText(ui->listWidget_eventSelection->currentItem()->text());
        ui->lineEdit_Overview_countPlaygrounds->setText(query.value(0).toString());
        ui->lineEdit_Overview_countPassages->setText(query.value(1).toString());
        ui->lineEdit_Overview_countTeams->setText(query.value(2).toString());
        ui->lineEdit_Overview_host->setText(query.value(3).toString());
        ui->lineEdit_Overview_exec->setText(query.value(4).toString());
        ui->lineEdit_Overview_location->setText(query.value(5).toString());
        ui->lineEdit_Overview_dateAndTime->setText(query.value(6).toString());
        ui->lineEdit_Overview_type->setText(Global::matchTypeToQstring(matchType));
        ui->lineEdit_Overview_class->setText(Global::matchClassToQString(matchClass));
        ui->lineEdit_Overview_leader->setText(query.value(9).toString());
        ui->lineEdit_Overview_referee->setText(query.value(10).toString());
        ui->lineEdit_Overview_secretary->setText(query.value(11).toString());
    }else{
        ui->lineEdit_Overview_eventName->clear();
        ui->lineEdit_Overview_countPlaygrounds->clear();
        ui->lineEdit_Overview_countPassages->clear();
        ui->lineEdit_Overview_countTeams->clear();
        ui->lineEdit_Overview_host->clear();
        ui->lineEdit_Overview_exec->clear();
        ui->lineEdit_Overview_location->clear();
        ui->lineEdit_Overview_dateAndTime->clear();
        ui->lineEdit_Overview_type->clear();
        ui->lineEdit_Overview_class->clear();
        ui->lineEdit_Overview_leader->clear();
        ui->lineEdit_Overview_referee->clear();
        ui->lineEdit_Overview_secretary->clear();
    }
}

void MainWindow::onEventSelectionChanged()
{
    // Call all update functions
    updateEventOverview();
    updateEventTeamView();
}

void MainWindow::updateEventTeamView()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch Date of the selected event
    //-------------------------------------------------------------------------------------------------------------------------------
    QString selectedEvent = ui->listWidget_eventSelection->currentItem()->text();

    QSqlQuery query;
    query.exec(tr("SELECT tid,team_name FROM teams WHERE event_name='%1'").arg(selectedEvent));

    //-------------------------------------------------------------------------------------------------------------------------------
    // Update UI
    //-------------------------------------------------------------------------------------------------------------------------------
    ui->tableWidget_teamConfig->setRowCount(0);
    while (query.next()) {
        // Append new row
        ui->tableWidget_teamConfig->insertRow(ui->tableWidget_teamConfig->rowCount());

        // Set Column ID
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEditable);
        item->setText(query.value(0).toString());

        ui->tableWidget_teamConfig->setItem(ui->tableWidget_teamConfig->rowCount() - 1, 0, item);

        // Set Column Name
        ui->tableWidget_teamConfig->setItem(ui->tableWidget_teamConfig->rowCount() - 1, 1, new QTableWidgetItem(query.value(1).toString()));
    }
}

void MainWindow::onEventTeamNameChanged()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    if(ui->tableWidget_teamConfig->currentColumn() >= 0 && ui->tableWidget_teamConfig->currentRow() >= 0){
        int currentRow = ui->tableWidget_teamConfig->currentRow();

        QString eventName = ui->listWidget_eventSelection->currentItem()->text();
        QString tid = ui->tableWidget_teamConfig->item(currentRow,0)->text();
        QString teamName = ui->tableWidget_teamConfig->item(currentRow,1)->text();

        QSqlQuery query;
        query.exec(tr("UPDATE teams SET team_name='%1' WHERE tid='%2' AND event_name='%3'").arg(teamName, tid, eventName));
    }
}

void MainWindow::on_actionRules_triggered()
{
    QDesktopServices::openUrl(QUrl(QDir::currentPath() + "/rules.pdf"));
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_pushButton_eventCreate_clicked()
{
    eventWindow->show();
}

void MainWindow::on_pushButton_EventDel_clicked()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Check validity
    //-------------------------------------------------------------------------------------------------------------------------------
    if(ui->listWidget_eventSelection->selectedItems().isEmpty()){
        QMessageBox::warning(this, tr("Warnung"), tr("Keine Veranstaltung ausgewählt!"));
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Delete selected event from Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    query.exec(tr("DELETE FROM events WHERE event_name='%1'").arg(ui->listWidget_eventSelection->currentItem()->text()));
    query.exec(tr("DELETE FROM matches WHERE event_name='%1'").arg(ui->listWidget_eventSelection->currentItem()->text()));
    query.exec(tr("DELETE FROM teams WHERE event_name='%1'").arg(ui->listWidget_eventSelection->currentItem()->text()));
    updateEventSelection();
}
