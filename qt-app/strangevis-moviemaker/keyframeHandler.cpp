#include "keyframeHandler.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>


void KeyframeHandler::saveState(QString filename, QList<float*> matrices)
{
    QString f = QString("states/%1_state.txt").arg(filename);
    QFile file(f);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    foreach(auto& x, matrices) {
        QList<float> matrix;
        for (int i = 0; i < 16; i++) {
            matrix.append(x[i]);
            out << QString::number(x[i]) << " ";
        }
    }
    qDebug() << "Saved state to file: " << f;
    file.close();
}
