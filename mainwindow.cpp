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

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Constructor/Destructor
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
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

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Custom Slots (for updating/init./etc.)
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
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
    updateMainPassageSelection();
    updatePassageSelection();
}

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Menubar Slots
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
void MainWindow::on_actionRules_triggered()
{
    QDesktopServices::openUrl(QUrl(QDir::currentPath() + "/rules.pdf"));
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Slots for Tab Overview
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
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

void MainWindow::on_lineEdit_Overview_host_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString host = ui->lineEdit_Overview_host->text();

    query.exec(tr("UPDATE events SET host='%1' WHERE event_name='%2'").arg(host,currentEvent));
}

void MainWindow::on_lineEdit_Overview_exec_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString exec = ui->lineEdit_Overview_exec->text();

    query.exec(tr("UPDATE events SET exec='%1' WHERE event_name='%2'").arg(exec,currentEvent));
}

void MainWindow::on_lineEdit_Overview_location_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString location = ui->lineEdit_Overview_location->text();

    query.exec(tr("UPDATE events SET location='%1' WHERE event_name='%2'").arg(location,currentEvent));
}

void MainWindow::on_lineEdit_Overview_dateAndTime_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString dateAndTime = ui->lineEdit_Overview_dateAndTime->text();

    query.exec(tr("UPDATE events SET date_and_time='%1' WHERE event_name='%2'").arg(dateAndTime,currentEvent));
}

void MainWindow::on_lineEdit_Overview_leader_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString leader = ui->lineEdit_Overview_leader->text();

    query.exec(tr("UPDATE events SET leader='%1' WHERE event_name='%2'").arg(leader,currentEvent));
}

void MainWindow::on_lineEdit_Overview_referee_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString referee = ui->lineEdit_Overview_referee->text();

    query.exec(tr("UPDATE events SET referee='%1' WHERE event_name='%2'").arg(referee,currentEvent));
}

void MainWindow::on_lineEdit_Overview_secretary_editingFinished()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changed Item in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;
    QString currentEvent = ui->lineEdit_Overview_eventName->text();
    QString secretary = ui->lineEdit_Overview_secretary->text();

    query.exec(tr("UPDATE events SET secretary='%1' WHERE event_name='%2'").arg(secretary,currentEvent));
}

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Slots for Tab Teams
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
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

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Slots for Tab Results
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
void MainWindow::updateMainPassageSelection()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Init. Selection
    //-------------------------------------------------------------------------------------------------------------------------------
    ui->comboBox_mainPassageSelection->clear();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch Data from Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QString selectedEvent = ui->listWidget_eventSelection->currentItem()->text();
    QVector<int> mainPassages;

    QSqlQuery query;
    query.exec(tr("SELECT main_passage FROM matches WHERE event_name='%1' ORDER BY main_passage").arg(selectedEvent));

    while (query.next()) {
        if(!mainPassages.contains(query.value(0).toInt())){
            mainPassages.append(query.value(0).toInt());
            ui->comboBox_mainPassageSelection->addItem(query.value(0).toString(),query.value(0).toInt());
        }
    }
}

void MainWindow::updatePassageSelection()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Init. Selection
    //-------------------------------------------------------------------------------------------------------------------------------
    ui->comboBox_passageSelection->clear();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch Data from Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QString selectedEvent = ui->listWidget_eventSelection->currentItem()->text();
    QVector<int> passages;

    QSqlQuery query;
    query.exec(tr("SELECT passage FROM matches WHERE event_name='%1' ORDER BY passage").arg(selectedEvent));

    while (query.next()) {
        if(!passages.contains(query.value(0).toInt())){
            passages.append(query.value(0).toInt());
            ui->comboBox_passageSelection->addItem(query.value(0).toString(),query.value(0).toInt());
        }
    }
}

void MainWindow::updateResultTable()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Init.
    //-------------------------------------------------------------------------------------------------------------------------------
    QString eventName = ui->listWidget_eventSelection->currentItem()->text();
    QString selectedMainPassage = ui->comboBox_mainPassageSelection->currentData().toString();
    QString selectedPassage = ui->comboBox_passageSelection->currentData().toString();

    // Delete table
    ui->tableWidget_inputResults->setRowCount(0);
    ui->tableWidget_inputResults->setColumnHidden(1, true);
    ui->tableWidget_inputResults->setColumnHidden(5, true);

    //-------------------------------------------------------------------------------------------------------------------------------
    // Check validity
    //-------------------------------------------------------------------------------------------------------------------------------
    if(eventName.isEmpty()){
        return;
    }

    if(selectedMainPassage.toInt() <= 0){
        return;
    }

    if(selectedPassage.toInt() <= 0){
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Fetch data from database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery queryMatch;

    queryMatch.exec(tr("SELECT tid_1,tid_2,playground,penalty_t1,penalty_t2,score_t1,score_t2 FROM matches WHERE event_name='%1' AND main_passage='%2' AND passage='%3' ORDER BY playground").arg(eventName,selectedMainPassage,selectedPassage));

    while(queryMatch.next()){
        // Append new row
        ui->tableWidget_inputResults->insertRow(ui->tableWidget_inputResults->rowCount());

        int currentRow = ui->tableWidget_inputResults->rowCount() - 1;

        // Set tid
        ui->tableWidget_inputResults->setItem(currentRow, 1, new QTableWidgetItem(queryMatch.value(0).toString()));
        ui->tableWidget_inputResults->setItem(currentRow, 5, new QTableWidgetItem(queryMatch.value(1).toString()));

        // Set Playground
        QTableWidgetItem *itemPg = new QTableWidgetItem();
        itemPg->setFlags(Qt::ItemIsEditable);
        itemPg->setText(queryMatch.value(2).toString());
        ui->tableWidget_inputResults->setItem(currentRow, 0, itemPg);

        // Set
        ui->tableWidget_inputResults->setItem(currentRow, 4, new QTableWidgetItem(queryMatch.value(3).toString()));

        // Set Team name
        QSqlQuery queryTeams;

        // Name Team 1
        queryTeams.exec(tr("SELECT team_name FORM teams WHERE event_name='%1' AND tid='%2'").arg(eventName,queryMatch.value(0).toString()));
        if(queryTeams.next()){
            QTableWidgetItem *itemT1Name = new QTableWidgetItem();
            itemT1Name->setFlags(Qt::ItemIsEditable);
            itemT1Name->setText(queryTeams.value(0).toString());
            ui->tableWidget_inputResults->setItem(currentRow,2, itemT1Name);
        }
        // Name Team 2
        queryTeams.exec(tr("SELECT team_name FORM teams WHERE event_name='%1' AND tid='%2'").arg(eventName,queryMatch.value(1).toString()));
        if(queryTeams.next()){
            QTableWidgetItem *itemT1Name = new QTableWidgetItem();
            itemT1Name->setFlags(Qt::ItemIsEditable);
            itemT1Name->setText(queryTeams.value(0).toString());
            ui->tableWidget_inputResults->setItem(currentRow,6, itemT1Name);
        }
    }
}

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Slots for Tab Outputs
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================

void MainWindow::on_comboBox_mainPassageSelection_currentIndexChanged(int index)
{
    updateResultTable();
}

void MainWindow::on_comboBox_passageSelection_currentIndexChanged(int index)
{
    updateResultTable();
}
