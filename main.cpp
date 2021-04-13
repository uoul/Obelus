#include "mainwindow.h"
#include "global.h"

#include <QApplication>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //-------------------------------------------------------------------------------------------------------------------------------
    // Connect to Database
    //-------------------------------------------------------------------------------------------------------------------------------
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("main.db");
    if (!db.open()) {
        qCritical() << "Keine Verbindung zur Datenbank main.db möglich!";
        return 1;
    }
    Global::createDBifNotExists();

    //-------------------------------------------------------------------------------------------------------------------------------
    // Show MainWindow
    //-------------------------------------------------------------------------------------------------------------------------------
    MainWindow w;
    w.show();
    return a.exec();
}
