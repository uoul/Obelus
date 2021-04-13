#include "mytemplate.h"

MyTemplate::MyTemplate(QString s)
{
    myTemplateString = s;
}

QString MyTemplate::getMyTemplate()
{
    return this->myTemplateString;
}

QString MyTemplate::render(QHash<QString, QString> hash)
{
    // declare Variables
    QString renderedTemplate(myTemplateString);

    // Loop over hash
    QHash<QString, QString>::iterator i;
    for (i = hash.begin(); i != hash.end(); ++i){
        renderedTemplate.replace(i.key(), i.value());
    }

    return renderedTemplate;
}
