#include "keyframeHandler.h"
#include "strangevismoviemaker.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QSize>
#include <QDesktopWidget>


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
    QSize* height = new QSize(QDesktopWidget().availableGeometry().width() * 0.15, QDesktopWidget().availableGeometry().width() * 0.15);
    QString f = QString("states/%1_snapshot_%2.png").arg(filename, QString::number(numberofStates));
    widget->grab().scaledToHeight(height->height() * 0.3).save(f);
    qDebug() << "Saved snapshot: " << f;
}

QWidget* KeyframeHandler::updateKeyframes(QWidget* keyframeWrapper, QSize* square) {
    if (keyframeWrapper->layout() != NULL)
    {
        QLayoutItem* item;
        while ((item = keyframeWrapper->layout()->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
        delete keyframeWrapper->layout();
    }
    QGridLayout* keyframeGrid = new QGridLayout();

    keyframeWrapper->heightForWidth(true);

    int row = 2;
    int col = 1;
    QDir directory("states/");
    QStringList images = directory.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
    std::reverse(images.begin(), images.end());
    int imLength = images.length();
    for (int i = 0; i < 8; i++) {
        auto k = new QWidget();
        k->setFixedSize(*square * 0.3);
        if (i < imLength) {
            QString path = "background-image: url(./states/";
            path.append(images[i]);
            path.append("); background-position: center;");
            k->setStyleSheet(path);
        }
        else {
            k->setStyleSheet("background-color: #C4C4C4;");
        }
        keyframeGrid->addWidget(k, row, col);
        if (col == 0) {
            col = 2;
            row--;
        }
        else {
            col--;
        }

    }
    keyframeWrapper->setLayout(keyframeGrid);
    return keyframeWrapper;
}