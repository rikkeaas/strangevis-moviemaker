#include "keyframeHandler.h"
#include "strangevismoviemaker.h"
#include "keyframes.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QSize>
#include <QDesktopWidget>


void KeyframeHandler::saveState(QWidget* widget, QString filename, QList<float*> matrices, QVector3D backgroundColor, QVector<float> phaseFunctionData)
{
    setFilenameNumber();
    numberofStates++;
    QString n = QStringLiteral("%1").arg(numberofStates, 2, 10, QLatin1Char('0'));
    QString f = QString("states/%1_state_%2.txt").arg(filename, n);
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

    out << QString::number(backgroundColor.x()) << "\n";
    out << QString::number(backgroundColor.y()) << "\n";
    out << QString::number(backgroundColor.z()) << "\n";

    for (int i = 0; i < 256 * 4; i++) {
        float f = phaseFunctionData.at(i);
        out << QString::number(f) << "\n";
    }
    
    qDebug() << "Saved state to file: " << f;
}

void KeyframeHandler::takeQtScreenShot(QWidget* widget, QString filename) {
    QSize* height = new QSize(QDesktopWidget().availableGeometry().width() * 0.2, QDesktopWidget().availableGeometry().width() * 0.2);
    QString n = QStringLiteral("%1").arg(numberofStates, 2, 10, QLatin1Char('0'));
    QString f = QString("states/%1_snapshot_%2.png").arg(filename, n);
    widget->grab().scaledToHeight(height->height() * 0.3).save(f);
    qDebug() << "Saved snapshot: " << f;
}


QWidget* KeyframeHandler::updateKeyframes(QWidget* keyframeWrapper, QSize* square, QString filename) {
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
    this->filename = filename;
    QGridLayout* keyframeGrid = new QGridLayout();
    keyframeWrapper->heightForWidth(true);

    int row = 2;
    int col = 1;

    QList<QStringList> files = getFiles();
    QStringList images = files.at(0);
    QStringList states = files.at(1);

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

void KeyframeHandler::highlightKeyframe(QWidget* keyframeWrapper, int index)
{
    QString yellowBorder = "border: 7px solid yellow;";
    backupStyleSheet = keyframeWrapper->layout()->itemAt(index)->widget()->styleSheet();
    keyframeWrapper->layout()->itemAt(index)->widget()->setStyleSheet(backupStyleSheet + yellowBorder);
}

void KeyframeHandler::removeKeyframeHighlighting(QWidget* keyframeWrapper, int index)
{
    keyframeWrapper->layout()->itemAt(index)->widget()->setStyleSheet(backupStyleSheet);
}

void KeyframeHandler::readStates(QString statePath) {
    QFile inputFile(statePath);
    QList<float> textFileLine;
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            double dd = line.toFloat();
            textFileLine.append(dd);
        }
        inputFile.close();
    }
    
    if (textFileLine.length() > 0) {
        int point = 0;
        QList<QMatrix4x4> m_out;
        m_out.append(QMatrix4x4(textFileLine[0], textFileLine[1], textFileLine[2], textFileLine[3], textFileLine[4], textFileLine[5], textFileLine[6], textFileLine[7], textFileLine[8], textFileLine[9], textFileLine[10], textFileLine[11], textFileLine[12], textFileLine[13], textFileLine[14], textFileLine[15]).transposed());
        m_out.append(QMatrix4x4(textFileLine[16], textFileLine[17], textFileLine[18], textFileLine[19], textFileLine[20], textFileLine[21], textFileLine[22], textFileLine[23], textFileLine[24], textFileLine[25], textFileLine[26], textFileLine[27], textFileLine[28], textFileLine[29], textFileLine[30], textFileLine[31]).transposed());
        m_out.append(QMatrix4x4(textFileLine[32], textFileLine[33], textFileLine[34], textFileLine[35], textFileLine[36], textFileLine[37], textFileLine[38], textFileLine[39], textFileLine[40], textFileLine[41], textFileLine[42], textFileLine[43], textFileLine[44], textFileLine[45], textFileLine[46], textFileLine[47]).transposed());
        m_out.append(QMatrix4x4(textFileLine[48], textFileLine[49], textFileLine[50], textFileLine[51], textFileLine[52], textFileLine[53], textFileLine[54], textFileLine[55], textFileLine[56], textFileLine[57], textFileLine[58], textFileLine[59], textFileLine[60], textFileLine[61], textFileLine[62], textFileLine[63]).transposed());
        QVector3D backgroundColor = QVector3D(textFileLine[64], textFileLine[65], textFileLine[66]);
        QVector<float> phaseFunction;
        for (int i = 0; i < 1024; i++) {
            phaseFunction << textFileLine[i + 67];
        }
        matricesUpdated(m_out, backgroundColor, phaseFunction);
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

QList<QStringList> KeyframeHandler::getFiles() {
    QList<QStringList> out;
    QDir directory("states/");
    directory.setSorting(QDir::Name);
    QString imString = filename;
    imString.append("_snapshot_*.png");
    QString sString = filename;
    sString.append("_state_*.txt");
    QStringList images = directory.entryList(QStringList() << imString, QDir::Files);
    QStringList states = directory.entryList(QStringList() << sString, QDir::Files);
    out.append(images);
    out.append(states);
    return out;
}

void KeyframeHandler::setFilenameNumber() {
    QStringList states = getFiles().at(1);
    if (!states.isEmpty()) {

        qDebug() << states;
        int highest = 0;
        for (int f = 0; f < states.length(); f++) {
            QString file = states[states.length() - 1];
            int indexOfUnderScore = file.lastIndexOf("_");
            int indexOfDot = file.lastIndexOf(".");
            QString out;
            for (int i = indexOfUnderScore + 1; i < indexOfDot; i++) {
                out += file.at(i);
            }
            if (out.toInt() > highest) {
                highest = out.toInt();
            }
        }
        numberofStates = highest;
    }
    else {
        numberofStates = 0;
    }
}