#include "keyframeHandler.h"
#include "strangevismoviemaker.h"
#include "keyframes.h"
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
    file.close();
}

void KeyframeHandler::takeQtScreenShot(QWidget* widget, QString filename) {
    QSize* height = new QSize(QDesktopWidget().availableGeometry().width() * 0.2, QDesktopWidget().availableGeometry().width() * 0.2);
    QString f = QString("states/%1_snapshot_%2.png").arg(filename, QString::number(numberofStates));
    widget->grab().scaledToHeight(height->height() * 0.3).save(f);
    qDebug() << "Saved snapshot: " << f;
}



QWidget* KeyframeHandler::updateKeyframes(QWidget* keyframeWrapper, QSize* square) {
    if (keyframeWrapper->layout() != NULL)
    {
        QLayoutItem* item;
        int counter = 0;
        while ((item = keyframeWrapper->layout()->takeAt(0)) != NULL && counter < 8)
        {
            delete item->widget();
            delete item;
            counter++;
        }
        delete keyframeWrapper->layout();
    }

    QGridLayout* keyframeGrid = new QGridLayout();
    keyframeWrapper->heightForWidth(true);

    int row = 2;
    int col = 1;
    QDir directory("states/");
    QStringList images = directory.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
    QStringList states = directory.entryList(QStringList() << "*.txt" << "*.TXT", QDir::Files);

    std::reverse(images.begin(), images.end());
    std::reverse(states.begin(), states.end());

    numberofStates = images.length();
    for (int i = 0; i < 8; i++) {
        auto k = new Keyframe(this);
        QObject::connect(k, &Keyframe::clicked, this, &KeyframeHandler::readStates);
        QObject::connect(k, &Keyframe::clickForRemove, this, &KeyframeHandler::deleteKeyframe);
        k->setFixedSize(*square * 0.3);
        if (i < numberofStates) {
            QString statePath = "./states/";
            statePath.append(states[i]);
            k->setStatePath(statePath);
            QString snapshotPath("./states/");
            snapshotPath.append(images[i]);
            k->setSnapshotPath(snapshotPath);
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

    auto add = new AddButton(this);
    QObject::connect(add, &AddButton::clicked, this, &KeyframeHandler::addButton);
    QGridLayout* layout = new QGridLayout();
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            auto w = new QWidget();
            if (i == 2 || j == 2) {
                w->setStyleSheet("background: #C4C4C4;");
            }
            else {
                w->setStyleSheet("background: transparent;");
            }
            layout->addWidget(w, i, j);
        }
    }
    layout->setSpacing(0);
    add->setStyleSheet(":hover{background: #323232}");
    add->setLayout(layout);
    add->setContentsMargins(13, 13, 13, 13);
    add->setFixedSize(*square * 0.3);
    keyframeGrid->addWidget(add, 2, 2);
    keyframeWrapper->setLayout(keyframeGrid);
    return keyframeWrapper;
}

void KeyframeHandler::readStates(QString statePath) {
    QFile inputFile(statePath);
    QList<float> matrices;
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            double dd = line.toFloat();
            matrices.append(dd);
        }
        inputFile.close();
    }
    
    if (matrices.length() > 0) {
        int point = 0;
        QList<QMatrix4x4> m_out;
        m_out.append(QMatrix4x4(matrices[0], matrices[1], matrices[2], matrices[3], matrices[4], matrices[5], matrices[6], matrices[7], matrices[8], matrices[9], matrices[10], matrices[11], matrices[12], matrices[13], matrices[14], matrices[15]).transposed());
        m_out.append(QMatrix4x4(matrices[16], matrices[17], matrices[18], matrices[19], matrices[20], matrices[21], matrices[22], matrices[23], matrices[24], matrices[25], matrices[26], matrices[27], matrices[28], matrices[29], matrices[30], matrices[31]).transposed());
        m_out.append(QMatrix4x4(matrices[32], matrices[33], matrices[34], matrices[35], matrices[36], matrices[37], matrices[38], matrices[39], matrices[40], matrices[41], matrices[42], matrices[43], matrices[44], matrices[45], matrices[46], matrices[47]).transposed());
        m_out.append(QMatrix4x4(matrices[48], matrices[49], matrices[50], matrices[51], matrices[52], matrices[53], matrices[54], matrices[55], matrices[56], matrices[57], matrices[58], matrices[59], matrices[60], matrices[61], matrices[62], matrices[63]).transposed());
        matricesUpdated(m_out);
    }
}

void KeyframeHandler::deleteKeyframe(QString statePath, QString snapshotPath)
{
    if (!statePath.isEmpty() && !snapshotPath.isEmpty()) {
        QFile(statePath).remove();
        QFile(snapshotPath).remove();
    }
    deletedKeyframe();
}

void KeyframeHandler::addButton() {
    addedKeyframe();
}