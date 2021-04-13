#include "global.h"
#include <QString>
#include <QDebug>

//###############################################################################################################################
// Constructor
//###############################################################################################################################


//###############################################################################################################################
// Divers Functions
//###############################################################################################################################
Global::Global()
{

}

void Global::createDBifNotExists()
{
    //-------------------------------------------------------------------------------------------------------------------------------
    // Declare Variables
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlQuery query;

    //-------------------------------------------------------------------------------------------------------------------------------
    // Create Tables
    //-------------------------------------------------------------------------------------------------------------------------------

    // Eventtable
    query.exec("CREATE TABLE 'events' ('event_name'	TEXT NOT NULL UNIQUE,'count_playgrounds'	INTEGER NOT NULL,'count_passages'	INTEGER NOT NULL,'count_teams'	INTEGER NOT NULL,'host'	TEXT,'exec'	TEXT,'location'	TEXT,'date_and_time'	TEXT,'type'	INTEGER,'class'	INTEGER,'leader'	TEXT,'referee'	TEXT,'secretary'	TEXT,PRIMARY KEY('event_name'))");

    // Match Table
    query.exec("CREATE TABLE 'matches' ('event_name'	TEXT NOT NULL,'main_passage'	INTEGER NOT NULL,'tid_1'	INTEGER NOT NULL,'tid_2'	INTEGER NOT NULL,'passage'	INTEGER NOT NULL,'playground'	INTEGER NOT NULL,'start_tid'	INTEGER NOT NULL,'penalty_t1'	INTEGER,'penalty_t2'	INTEGER,'score_t1'	INTEGER,'score_t2'	INTEGER,PRIMARY KEY('event_name','main_passage','tid_1','tid_2'))");

    // Teams Table
    query.exec("CREATE TABLE 'teams' ('event_name'	TEXT NOT NULL,'tid'	INTEGER NOT NULL,'team_name'	TEXT NOT NULL,PRIMARY KEY('event_name','tid','team_name'))");
}

QString Global::matchClassToQString(matchClass mc)
{
    QString out;
    out.clear();

    switch (mc) {
    case u14:
        out = "Schüler/Jugend U14";
        break;

    case u16:
        out = "Jugend U16";
        break;

    case u19:
        out = "Jugend U19";
        break;

    case u23:
        out = "Junioren U23";
        break;

    case ManAndWoman:
        out = "Damen- und Herren";
        break;

    case senior:
        out = "Senioren";
        break;

    case mixed:
        out = "Mixed";
        break;

    case solo:
        out = "Solo";
        break;

    case duo:
        out = "Duo";
        break;

    case trio:
        out = "Trio";
        break;

    }

    return out;
}

QString Global::matchTypeToQstring(matchType mt)
{
    QString out;
    out.clear();

    switch (mt) {
    case RoundRobin:
        out = "Jeder-gegen-Jeden";
        break;
    }

    return out;
}
