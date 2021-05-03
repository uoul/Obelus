#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "mytemplate.h"

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
    initIntermediateResultDir();

    // connect signals & slots
    connect(eventWindow,SIGNAL(eventCreated()),this,SLOT(updateEventSelection()));
    connect(ui->listWidget_eventSelection,SIGNAL(itemSelectionChanged()),this,SLOT(onEventSelectionChanged()));
    connect(ui->tableWidget_teamConfig,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(onEventTeamNameChanged()));
    connect(ui->tableWidget_inputResults,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(onEventResultChanged()));
}

MainWindow::~MainWindow()
{
    // Delete stuff on heap
    delete ui;
    delete eventWindow;

    // init Temp. Dir
    initIntermediateResultDir();
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

void MainWindow::initIntermediateResultDir()
{
    QDir d(QDir::currentPath() + "/IntermediateResults");

    d.removeRecursively();
    d.mkdir(QDir::currentPath() + "/IntermediateResults");
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
        createIntermediateResultHtml();
    }
    updateResultTable();
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
        ui->tableWidget_inputResults->setItem(currentRow, 3, new QTableWidgetItem(queryMatch.value(5).toString()));     // Current score 1
        ui->tableWidget_inputResults->setItem(currentRow, 4, new QTableWidgetItem(queryMatch.value(3).toString()));     // Current penalty 1
        ui->tableWidget_inputResults->setItem(currentRow, 7, new QTableWidgetItem(queryMatch.value(6).toString()));     // Current score 2
        ui->tableWidget_inputResults->setItem(currentRow, 8, new QTableWidgetItem(queryMatch.value(4).toString()));     // Current penalty 2

        // Set CellColor
        if(ui->tableWidget_inputResults->item(currentRow, 3)->text().isEmpty()){
            ui->tableWidget_inputResults->item(currentRow, 3)->setBackground(Qt::white);
        }else{
            ui->tableWidget_inputResults->item(currentRow, 3)->setBackground(Qt::green);
        }

        if(ui->tableWidget_inputResults->item(currentRow, 4)->text().isEmpty()){
            ui->tableWidget_inputResults->item(currentRow, 4)->setBackground(Qt::white);
        }else{
            ui->tableWidget_inputResults->item(currentRow, 4)->setBackground(Qt::yellow);
        }

        if(ui->tableWidget_inputResults->item(currentRow, 7)->text().isEmpty()){
            ui->tableWidget_inputResults->item(currentRow, 7)->setBackground(Qt::white);
        }else{
            ui->tableWidget_inputResults->item(currentRow, 7)->setBackground(Qt::green);
        }

        if(ui->tableWidget_inputResults->item(currentRow, 8)->text().isEmpty()){
            ui->tableWidget_inputResults->item(currentRow, 8)->setBackground(Qt::white);
        }else{
            ui->tableWidget_inputResults->item(currentRow, 8)->setBackground(Qt::yellow);
        }

        // Set Team name
        QSqlQuery queryTeams;

        // Name Team 1
        queryTeams.exec(tr("SELECT team_name FROM teams WHERE event_name='%1' AND tid='%2'").arg(eventName,queryMatch.value(0).toString()));
        if(queryTeams.next()){
            QTableWidgetItem *itemT1Name = new QTableWidgetItem();
            itemT1Name->setFlags(Qt::ItemIsEditable);
            itemT1Name->setText(queryTeams.value(0).toString());
            ui->tableWidget_inputResults->setItem(currentRow,2, itemT1Name);
        }
        // Name Team 2
        queryTeams.exec(tr("SELECT team_name FROM teams WHERE event_name='%1' AND tid='%2'").arg(eventName,queryMatch.value(1).toString()));
        if(queryTeams.next()){
            QTableWidgetItem *itemT1Name = new QTableWidgetItem();
            itemT1Name->setFlags(Qt::ItemIsEditable);
            itemT1Name->setText(queryTeams.value(0).toString());
            ui->tableWidget_inputResults->setItem(currentRow,6, itemT1Name);
        }
    }
}

void MainWindow::onEventResultChanged()
{
    int currentCol = ui->tableWidget_inputResults->currentColumn();
    int currentRow = ui->tableWidget_inputResults->currentRow();
    QSqlQuery query;

    //-------------------------------------------------------------------------------------------------------------------------------
    // Save Changes
    //-------------------------------------------------------------------------------------------------------------------------------
    if(currentCol >= 0 && currentRow >= 0){
        QString eventName = ui->listWidget_eventSelection->currentItem()->text();
        QString mainPassage = ui->comboBox_mainPassageSelection->currentText();
        QString passage = ui->comboBox_passageSelection->currentText();
        QString playGround = ui->tableWidget_inputResults->item(currentRow, 0)->text();
        QString currentCellValue = ui->tableWidget_inputResults->item(currentRow, currentCol)->text();

        if(Global::qStringIsNumber(ui->tableWidget_inputResults->item(currentRow, currentCol)->text()) || currentCellValue.isEmpty()){
            if(currentCol == 3){            // score 1 changed
                query.exec(tr("UPDATE matches SET score_t1='%1' WHERE event_name='%2' AND main_passage='%3' AND passage='%4' AND playground='%5'").arg(currentCellValue,eventName,mainPassage,passage,playGround));
                if(!currentCellValue.isEmpty()){
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::green);
                }else{
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::white);
                }
            }else if (currentCol == 4) {    // penalty 1 changed
                query.exec(tr("UPDATE matches SET penalty_t1='%1' WHERE event_name='%2' AND main_passage='%3' AND passage='%4' AND playground='%5'").arg(currentCellValue,eventName,mainPassage,passage,playGround));
                if(currentCellValue.toInt() != 0){
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::yellow);
                }else{
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setText("");
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::white);
                }
            }else if (currentCol == 7){     // score 2 changed
                query.exec(tr("UPDATE matches SET score_t2='%1' WHERE event_name='%2' AND main_passage='%3' AND passage='%4' AND playground='%5'").arg(currentCellValue,eventName,mainPassage,passage,playGround));
                if(!currentCellValue.isEmpty()){
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::green);
                }else{
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::white);
                }
            }else if (currentCol == 8) {    // penalty 2 changed
                query.exec(tr("UPDATE matches SET penalty_t2='%1' WHERE event_name='%2' AND main_passage='%3' AND passage='%4' AND playground='%5'").arg(currentCellValue,eventName,mainPassage,passage,playGround));
                if(currentCellValue.toInt() != 0){
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::yellow);
                }else{
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setText("");
                    ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::white);
                }
            }
        }else{
            ui->tableWidget_inputResults->item(currentRow, currentCol)->setText("");
            ui->tableWidget_inputResults->item(currentRow, currentCol)->setBackground(Qt::white);
        }
        createIntermediateResultHtml();
    }
}

void MainWindow::on_comboBox_mainPassageSelection_currentIndexChanged(int index)
{
    int *i = new int;
    *i = index;
    delete i;
    updateResultTable();
}

void MainWindow::on_comboBox_passageSelection_currentIndexChanged(int index)
{
    int *i = new int;
    *i = index;
    delete i;
    updateResultTable();
}

//===============================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------------
// Slots for Tab Outputs
//-------------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================================
void MainWindow::createIntermediateResult()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Init.
    //-------------------------------------------------------------------------------------------------------------------------------
    QString currentEvent = ui->listWidget_eventSelection->currentItem()->text();
    QSqlQuery query;

    //-------------------------------------------------------------------------------------------------------------------------------
    // Check validity
    //-------------------------------------------------------------------------------------------------------------------------------
    if(currentEvent.isEmpty()){
        QMessageBox::warning(this, tr("Warnung"), tr("Keine Veranstaltung ausgewählt!"));
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create qHash (Hash with all teams and their score of the selected event)
    //-------------------------------------------------------------------------------------------------------------------------------
    struct resultData {
        int rank;
        int matchPointsWon;
        int matchPointsLost;
        int pointsWon;
        int pointsLost;
        double quota;
    };
    QHash<int, resultData> result;

    query.exec(tr("SELECT tid FROM teams WHERE event_name='%1'").arg(currentEvent));

    while (query.next()) {
        resultData d;
        d.rank = 0;
        d.matchPointsWon = 0;
        d.matchPointsLost = 0;
        d.pointsWon = 0;
        d.pointsLost = 0;
        d.quota = 0.0;

        result.insert(query.value(0).toInt(), d);
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Loop over matches and count
    //-------------------------------------------------------------------------------------------------------------------------------
    query.exec(tr("SELECT tid_1,tid_2,score_t1,score_t2,penalty_t1,penalty_t2 FROM matches WHERE event_name='%1'").arg(currentEvent));

    while (query.next()) {
        int tid1 = query.value(0).toInt();
        int tid2 = query.value(1).toInt();
        int scoreT1 = query.value(2).toInt();
        int scoreT2 = query.value(3).toInt();
        int penaltyT1 = query.value(4).toInt();
        int penaltyT2 = query.value(5).toInt();
        int sumT1 = scoreT1 - penaltyT1;
        int sumT2 = scoreT2 - penaltyT2;

        // Correct score if neccesary
        if(sumT1 == 0 and sumT2 == 0){
            continue;
        }

        if(sumT1 < 0){
            sumT2 = sumT2 + sumT1 * -1;
            sumT1 = 0;
        }

        if(sumT2 < 0){
            sumT1 = sumT1 + sumT2 * -1;
            sumT2 = 0;
        }

        // Matchpoints
        if(sumT1 > sumT2){
            result[tid1].matchPointsWon += 2;
            result[tid2].matchPointsLost += 2;
        }else if (sumT1 < sumT2) {
            result[tid1].matchPointsLost += 2;
            result[tid2].matchPointsWon += 2;
        }else {
            result[tid1].matchPointsWon += 1;
            result[tid1].matchPointsLost += 1;
            result[tid2].matchPointsWon += 1;
            result[tid2].matchPointsLost += 1;
        }

        result[tid1].pointsWon += sumT1;
        result[tid1].pointsLost += sumT2;

        result[tid2].pointsWon += sumT2;
        result[tid2].pointsLost += sumT1;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Save result in database
    //-------------------------------------------------------------------------------------------------------------------------------
    QHash<int, resultData>::iterator i;
    for(i = result.begin(); i != result.end(); i++){
        QString rank = QString::number(i.value().rank);
        QString matchPointsWon = QString::number(i.value().matchPointsWon);
        QString matchPointsLost = QString::number(i.value().matchPointsLost);
        QString pointsWon = QString::number(i.value().pointsWon);
        QString pointsLost = QString::number(i.value().pointsLost);
        QString quota = QString::number(i.value().quota);

        // Calculate quota
        if(i.value().pointsWon != 0 || i.value().pointsLost != 0){
            if(i.value().pointsLost == 0){
                quota = "Infinity";
            }else{
                quota = QString::number((double)i.value().pointsWon/i.value().pointsLost);
            }
        }

        // Calculate rank


        query.exec(tr("UPDATE teams SET rank='%1',match_points_won='%2',match_points_lost='%3',points_won='%4',points_lost='%5',quota='%6' WHERE event_name='%7' AND tid='%8'").arg(rank,matchPointsWon,matchPointsLost,pointsWon,pointsLost,quota,currentEvent,QString::number(i.key())));
    }
}

void MainWindow::createIntermediateResultHtml()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // init.
    //-------------------------------------------------------------------------------------------------------------------------------
    QString selectedEvent = ui->listWidget_eventSelection->currentItem()->text();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Check validity
    //-------------------------------------------------------------------------------------------------------------------------------
    if(ui->listWidget_eventSelection->currentItem()->text().isEmpty()){
        QMessageBox::warning(this, tr("Warnung"), tr("Keine Veranstaltung ausgewählt!"));
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Update Result
    //-------------------------------------------------------------------------------------------------------------------------------
    createIntermediateResult();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create HTML Rows
    //-------------------------------------------------------------------------------------------------------------------------------
    MyTemplate rowTemplate("<tr><td>{rank}</td><td>{teamName}</td><td>{matchPointsWon}:{matchPointsLost}</td><td>{pointsWon}:{pointsLost}</td><td>{quota}</td></tr>");
    QString rowsHtml = "";
    QSqlQuery query;

    query.exec(tr("SELECT team_name,rank,match_points_won,match_points_lost,points_won,points_lost,quota FROM teams WHERE event_name='%1' ORDER BY rank").arg(selectedEvent));

    while(query.next()){
        QString teamName = query.value(0).toString();
        QString rank = query.value(1).toString();
        QString matchPointsWon = query.value(2).toString();
        QString matchPointsLost = query.value(3).toString();
        QString pointsWon = query.value(4).toString();
        QString pointsLost = query.value(5).toString();
        QString quota = query.value(6).toString();

        QHash<QString,QString> h;

        h.insert("{teamName}", teamName);
        h.insert("{rank}", rank);
        h.insert("{matchPointsWon}", matchPointsWon);
        h.insert("{matchPointsLost}", matchPointsLost);
        h.insert("{pointsWon}", pointsWon);
        h.insert("{pointsLost}", pointsLost);
        h.insert("{quota}", quota);

        rowsHtml = rowsHtml + rowTemplate.render(h);
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Read Template
    //-------------------------------------------------------------------------------------------------------------------------------
    QFile f(":/intermediateResult/Template/IntermediatResultTableTemplate.html");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QString tableTemplate = f.readAll();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create HTML
    //-------------------------------------------------------------------------------------------------------------------------------
    QString fileName = selectedEvent;
    fileName.replace(" ", "");

    MyTemplate intermediateResultTableTemplate(tableTemplate);

    QFile file(QDir::currentPath() + "/IntermediateResults/" + fileName + ".html");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            return;
    }

    QTextStream out(&file);

    QHash<QString,QString> h;
    h.insert("{eventName}",selectedEvent);
    h.insert("{tableRows}",rowsHtml);

    out << intermediateResultTableTemplate.render(h); 
}

void MainWindow::on_pushButton_showIntermediateResult_clicked()
{
    createIntermediateResultHtml();
    QDesktopServices::openUrl(QUrl(QDir::currentPath() + "/IntermediateResults/" + ui->listWidget_eventSelection->currentItem()->text().replace(" ","") + ".html"));
}
