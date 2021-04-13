#ifndef MYTEMPLATE_H
#define MYTEMPLATE_H

#include <QString>
#include <QHash>

class MyTemplate
{
public:
    MyTemplate(QString s);

    QString getMyTemplate();
    QString render(QHash<QString, QString>);

private:
    QString myTemplateString;
};

#endif // MYTEMPLATE_H
