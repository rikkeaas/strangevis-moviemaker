#include "keyframeHandler.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>


void KeyframeHandler::saveState(QString filename)
{
    QString f = QString("states/%1_state.txt").arg(filename);
    QFile file(f);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    out << "Test112";
    file.close();
    qDebug() << "Saved state to file: " << f;
}
