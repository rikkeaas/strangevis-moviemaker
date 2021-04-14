#include "keyframeHandler.h"
#include "strangevismoviemaker.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>

void KeyframeHandler::saveState(QWidget* widget, QString filename, QList<float*> matrices)
{
    numberofStates++;

    QString f = QString("states/%1_state_%2.txt").arg(filename, QString::number(numberofStates));
    QFile file(f);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    foreach(auto & x, matrices) {
        QList<float> matrix;
        for (int i = 0; i < 16; i++) {
            matrix.append(x[i]);
            out << QString::number(x[i]) << "\n";
        }
    }
    qDebug() << "Saved state to file: " << f;
    takeQtScreenShot(widget, filename);
    file.close();
}

void KeyframeHandler::takeQtScreenShot(QWidget* widget, QString filename) {
    QString f = QString("snapshots/%1_snapshot_%2.png").arg(filename, QString::number(numberofStates));
    widget->grab().save(f);
    qDebug() << "Saved snapshot: " << f;

}