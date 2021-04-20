#ifndef GLOBAL_H
#define GLOBAL_H
#include <QCoreApplication>
#include <QMessageBox>
#include <QSqlQuery>
#include <QString>

class Global
{
public:
    Global();

    enum matchType{
        RoundRobin
    };

    enum matchClass{
        u14,
        u16,
        u19,
        u23,
        ManAndWoman,
        senior,
        mixed,
        solo,
        duo,
        trio
    };

    void static createDBifNotExists();

    QString static matchTypeToQstring(matchType);

    QString static matchClassToQString(matchClass);

    bool static qStringIsNumber(const QString &s);
};

#endif // GLOBAL_H
