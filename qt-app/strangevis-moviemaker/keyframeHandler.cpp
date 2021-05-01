#include "keyframeHandler.h"
#include "strangevismoviemaker.h"
#include "keyframes.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QSize>
#include <QDesktopWidget>


void KeyframeHandler::saveState(QWidget* widget, QString filename, QList<float*> matrices, QVector3D backgroundColor, QVector<float> phaseFunctionData, QList<Layer*> layers)
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

    for (int i = 0; i < phaseFunctionData.length(); i++) {
        float f = phaseFunctionData.at(i);
        out << QString::number(f) << "\n";
    }

    if (layers.length() > 0) {
        foreach(Layer* x, layers) {
            QString layer = "";
            layer.append(x->label->text() + ",");
            qDebug() << "Writing rect: " << x->m_selectedArea;
            layer.append(QString::number(x->m_selectedArea.topLeft().x()) + ",");
            layer.append(QString::number(x->m_selectedArea.topLeft().y()) + ",");
            layer.append(QString::number(x->m_selectedArea.width()) + ",");
            layer.append(QString::number(x->m_selectedArea.height()) + ",");
            layer.append(QString::number(x->m_layerRGBA.alpha()) + ",");
            layer.append(QString::number(x->m_layerRGBA.red()) + ",");
            layer.append(QString::number(x->m_layerRGBA.green()) + ",");
            layer.append(QString::number(x->m_layerRGBA.blue()) + "\n");
            out << layer;
        }
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
    if (inputFile.open(QIODevice::ReadOnly))
    {
        float m_projectionMatrix[16];
        float m_rotateMatrix[16];
        float m_scaleMatrix[16];
        float m_translateMatrix[16];
        QVector3D m_backgroundColorVector;
        QVector<float> m_phasefunctionVector;
        QList<Layer*> m_layers;

        QTextStream in(&inputFile);
        int index = 0;
        int i = 0;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (i > 1024 + 66) {
                QList<QString> layerString = line.split(",");
                QString label = layerString[0];
                QRect rect = QRect(layerString[1].toInt(), layerString[2].toInt(), layerString[3].toInt(), layerString[4].toInt());
                qDebug() << "Reading rect: " << rect;
                QColor color = QColor();
                color.setAlpha(layerString[5].toInt());
                color.setRed(layerString[6].toInt());
                color.setGreen(layerString[7].toInt());
                color.setBlue(layerString[8].toInt());
                Layer* layer = new Layer(nullptr, rect, true, color);
                layer->label->setText(label);
                layer->m_selectedArea = rect;
                layer->m_layerRGBA = color;
                layer->setStyleSheet("background-color:#6D6D6D; height:45; border-radius:10px;");
                m_layers.append(layer);
            }
            else if (i > 66) {
                m_phasefunctionVector << line.toFloat();
            } else if (i == 64) {
                m_backgroundColorVector.setX(line.toFloat());
            } else if (i == 65) {
                m_backgroundColorVector.setY(line.toFloat());
            } else if (i == 66) {
                m_backgroundColorVector.setZ(line.toFloat());
            } else if (i > 47) {
                m_translateMatrix[index] = line.toFloat();
            } else if (i > 31) {
                m_scaleMatrix[index] = line.toFloat();
            } else if (i > 15) {
                m_rotateMatrix[index] = line.toFloat();
            } else {
                m_projectionMatrix[index] = line.toFloat();
            }
            i++;
            index++;
            if (index == 16) {
                index = 0;
            }
        }
        inputFile.close();

        QList<QMatrix4x4> m_out;
        m_out.append(QMatrix4x4(m_projectionMatrix).transposed());
        m_out.append(QMatrix4x4(m_rotateMatrix).transposed());
        m_out.append(QMatrix4x4(m_scaleMatrix).transposed());
        m_out.append(QMatrix4x4(m_translateMatrix).transposed());
        matricesUpdated(m_out, QVector3D(m_backgroundColorVector), m_phasefunctionVector, m_layers);
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