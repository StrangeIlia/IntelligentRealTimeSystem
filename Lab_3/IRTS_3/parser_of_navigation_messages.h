#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <QVector>
#include <QTextStream>
#include "satellite/navigation_message.h"

class ParserOfNavigationMessages
{
public:
    ParserOfNavigationMessages();

    NavigationMessage* convertBlock(QString str) const;
    QVector<NavigationMessage*> convertStream(QTextStream &stream);

private:
    QString getLine(QTextStream &stream) const;
    NavigationMessage* convertBlock(QTextStream &stream) const;
};

#endif // PARSER_H
