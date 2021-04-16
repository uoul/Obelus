#include "eventwindow.h"
#include "ui_eventwindow.h"
#include "global.h"
#include "slidesystem.h"

#include <QDebug>

EventWindow::EventWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventWindow)
{
    ui->setupUi(this);

    // Fill Matchtypselection
    ui->comboBox_type->addItem(Global::matchTypeToQstring(Global::matchType::RoundRobin), Global::matchType::RoundRobin);

    // Fill MatchClassSelection
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::u14), Global::matchClass::u14);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::u16), Global::matchClass::u16);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::u19), Global::matchClass::u19);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::u23), Global::matchClass::u23);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::ManAndWoman), Global::matchClass::ManAndWoman);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::senior), Global::matchClass::senior);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::mixed), Global::matchClass::mixed);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::solo), Global::matchClass::solo);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::duo), Global::matchClass::duo);
    ui->comboBox_class->addItem(Global::matchClassToQString(Global::matchClass::trio), Global::matchClass::trio);
}

EventWindow::~EventWindow()
{
    delete ui;
}

void EventWindow::initUi()
{
    ui->lineEdit_eventName->clear();
    ui->lineEdit_exec->clear();
    ui->lineEdit_host->clear();
    ui->lineEdit_leader->clear();
    ui->lineEdit_location->clear();
    ui->lineEdit_referee->clear();
    ui->lineEdit_secretary->clear();
}


void EventWindow::on_commandLinkButton_create_clicked()
{
    QSqlQuery query;
    //-------------------------------------------------------------------------------------------------------------------------------
    // Check validity
    //-------------------------------------------------------------------------------------------------------------------------------

    // eventname must be given
    if(ui->lineEdit_eventName->text().trimmed().isEmpty()){
        QMessageBox::warning(this, tr("Warnung"), tr("Veranstaltungsname darf nicht leer sein!"));
        return;
    }

    // check count Playgrounds
    if(ui->spinBox_countPlaygrounds->value() > ui->spinBox_countTeams->value() / 2){
        ui->spinBox_countPlaygrounds->setValue(ui->spinBox_countTeams->value()/2);
    }

    // check if event already exists
    query.exec(tr("SELECT * FROM events WHERE event_name='%1'").arg(ui->lineEdit_eventName->text()));
    if(query.next()){
        QMessageBox::warning(this, tr("Warnung"), tr("Veranstaltung existiert bereits! - Veranstaltungsname muss eindeutig sein!"));
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create Event in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QString event_name = ui->lineEdit_eventName->text();
    QString count_playgrounds = QString::number(ui->spinBox_countPlaygrounds->value());
    QString count_passages = QString::number(ui->spinBox_countPassages->value());
    QString count_teams = QString::number(ui->spinBox_countTeams->value());
    QString host = ui->lineEdit_host->text();
    QString exec = ui->lineEdit_exec->text();
    QString location = ui->lineEdit_location->text();
    QString date_and_time = ui->dateTimeEdit_dateAndTime->text();
    QString type = QString(ui->comboBox_type->currentData().toString());
    QString matchClass = QString(ui->comboBox_class->currentData().toString());
    QString leader = ui->lineEdit_leader->text();
    QString referee = ui->lineEdit_referee->text();
    QString secretary = ui->lineEdit_secretary->text();

    query.exec(tr("INSERT INTO events (event_name,count_playgrounds,count_passages,count_teams,host,exec,location,date_and_time,type,class,leader,referee,secretary) VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13')").arg(event_name,count_playgrounds,count_passages,count_teams,host,exec,location,date_and_time,type,matchClass,leader,referee,secretary));

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create teams in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    for(int i = 1; i <= ui->spinBox_countTeams->value(); i++){
        query.exec(tr("INSERT INTO teams (event_name,tid,team_name) VALUES ('%1','%2','%3')").arg(event_name,QString::number(i),""));
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create matches in Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QString mainPassage;
    QString tid1;
    QString tid2;
    QString passage;
    QString playground;
    QString startTid;
    QString penaltyT1;
    QString penaltyT2;
    QString scoreT1;
    QString scoreT2;

    // Create Slide System
    SlideSystem s(ui->spinBox_countTeams->value());

    // Create matches
    for(int mp = 1; mp <= ui->spinBox_countPassages->value(); mp++){
        // set current values
        mainPassage = QString::number(mp);
        s.resetIterator();
        QPair<int,int> currentMatch = s.getNextMatch();
        int pg = 1;
        int tpassage = 1;

        while (currentMatch != QPair<int,int>(0,0)) {
            // set current values
            tid1 = QString::number(currentMatch.first);
            tid2 = QString::number(currentMatch.second);
            passage = QString::number(tpassage);
            playground = QString::number(pg);
            startTid = QString::number(currentMatch.first);

            currentMatch = s.getNextMatch();
            pg++;
            if(pg > ui->spinBox_countPlaygrounds->value()){
                pg = 1;
                tpassage++;
            }
            query.exec(tr("INSERT INTO matches (event_name,main_passage,tid_1,tid_2,passage,playground,start_tid,penalty_t1,penalty_t2,score_t1,score_t2) VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')").arg(event_name,mainPassage,tid1,tid2,passage,playground,startTid,penaltyT1,penaltyT2,scoreT1,scoreT2));
        }
    }

    emit eventCreated();
    this->initUi();
    this->hide();
}
