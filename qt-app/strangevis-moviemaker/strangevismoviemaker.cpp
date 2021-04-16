#include "strangevismoviemaker.h"
#include "customSlider.h"
#include "histogram.h"
#include "keyframeHandler.h"
#include <QOpenGLWidget>
#include <QFileDialog>
#include <Qlabel>
#include <QPushButton>
#include "Renderer.h"
#include <QDockWIdget>
#include <QtWidgets/qgroupbox.h>
#include <QSize>
#include <QDesktopWidget>
#include <QtCharts>


strangevismoviemaker::strangevismoviemaker(Renderer* renderer, QWidget *parent)
    : QMainWindow(parent)
{
    renderer->setParent(this);
    m_renderer = renderer;

    ui.setupUi(this);
       
    QAction* fileOpenAction = new QAction("Open", this);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    ui.menuFile->addAction(fileOpenAction);

    QAction* saveStateAction = new QAction("Save Current State", this);
    connect(saveStateAction, SIGNAL(triggered()), this, SLOT(saveState()));
    ui.menuFile->addAction(saveStateAction);

    this->setMinimumSize(1600, 1200);

    auto* cw = ui.centralWidget->layout();
    cw->addWidget(m_renderer);

    qDebug() << ui.centralWidget->rect();

    appendDockWidgets();

    /*
    QWidget* container = new QWidget(this);
    container->setGeometry(QRect(0, 0, 1000, 1000));
    QVBoxLayout* contLayout = new QVBoxLayout(container);

    customSlider* slider = new customSlider(10, QRect(100, 50, 200, 16), 0, 300, container);
    customSlider* slider2 = new customSlider(2, QRect(100, 100, 300, 16), 0, 10, container);
    customSlider* slider3 = new customSlider(3, QRect(100, 150, 200, 16), 0, 255, container);

    contLayout->addWidget(slider);
    contLayout->addWidget(slider2);
    contLayout->addWidget(slider3);


    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(container);
    
    //layout->addWidget(new QPushButton("Test"));
    setCentralWidget(container);
    //this->setLayout(layout);
    this->setMinimumSize(QSize(1000, 1000));
    this->show();

    //ui.horizontalSlider->setStyleSheet("QSlider::groove:horizontal {color:black; background-color:red;}"
    //                                   "QSlider::handle:horizontal {background-color:blue; height:16px; width: 16px;}");
    //connect(ui.spinBox, SIGNAL(valueChanged(int)), ui.horizontalSlider, SLOT(setValue(int)));
    */
}

void strangevismoviemaker::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Volume File", QString(), "*.dat");


    if (!fileName.isEmpty())
    {
        if (m_renderer->getVolume()->load(fileName))
        {
            qDebug() << "Loaded volume " << fileName; 
        } else {
            qDebug() << "Failed to load volume " << fileName;
        }
    }
}

void strangevismoviemaker::saveState()
{
    m_renderer->setState();
    updateKeyframes();
}

void strangevismoviemaker::appendDockWidgets()
{
    qDebug() << "Volume contains values: " << !m_renderer->getVolume()->getDataset().isEmpty();

    auto dockLayout = new QVBoxLayout();
    auto dockContentWrapper = new QWidget();
    dockContentWrapper->setLayout(dockLayout);
    dockContentWrapper->setStyleSheet("background-color:#6D6D6D;");

    QDockWidget* toolbox = new QDockWidget(tr("Toolbox"), this);
    Histogram* h = new Histogram(m_renderer->getVolume()->getDataset());

    QWidget* histoWrapper = new QWidget();
    QVBoxLayout* histoLayout = new QVBoxLayout();
    histoLayout->addWidget(h->getHistogram());
    histoWrapper->setLayout(histoLayout);
    histoLayout->setContentsMargins(0, 0, 0, 0);

    dockLayout->addWidget(toolbarContent(histoWrapper, QString("Layers")));

    dockContentWrapper->setLayout(dockLayout);
    toolbox->setWidget(dockContentWrapper);
    formatDockWidgets(toolbox);
    this->addDockWidget(Qt::LeftDockWidgetArea, toolbox);

    QDockWidget* keyframes = new QDockWidget(tr("Keyframe Handler"), this);
    formatDockWidgets(keyframes);
    keyframeWrapper = new QWidget();
    updateKeyframes();
    keyframeWrapper->setFixedSize(*square);
    keyframeWrapper->setStyleSheet("background-color:#3C3C3C;");
    keyframes->setWidget(keyframeWrapper);
    keyframes->setMaximumWidth(QDesktopWidget().availableGeometry().width() * 0.15);
    this->addDockWidget(Qt::LeftDockWidgetArea, keyframes);
}

void strangevismoviemaker::updateKeyframes() {
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
    square = new QSize(QDesktopWidget().availableGeometry().width() * 0.15, QDesktopWidget().availableGeometry().width() * 0.15);

    keyframeWrapper->heightForWidth(true);

    int row = 2;
    int col = 1;
    QDir directory("snapshots/");
    QStringList images = directory.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
    std::reverse(images.begin(), images.end());
    int imLength = images.length();
    for (int i = 0; i < 8; i++) {
        auto k = new QWidget();
        k->setFixedSize(*square * 0.3);
        if (i < imLength) {
            QString path = "background-image: url(./snapshots/";
            path.append(images[i]);
            path.append("); background-position: center;");
            k->setStyleSheet(path);
        }
        else {
            k->setStyleSheet("background-color:#C4C4C4;");
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
}



void strangevismoviemaker::formatDockWidgets(QDockWidget* dw) {
    QDockWidget* dockWidget = dw;
    dw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dw->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dw->setFeatures(QDockWidget::DockWidgetMovable);
    QWidget* titleWidget = new QWidget(); /* where this a QMainWindow object */
    dw->setTitleBarWidget(titleWidget);
}

QWidget* strangevismoviemaker::toolbarContent(QWidget* content, QString header) {
    QVBoxLayout* dockLayout = new QVBoxLayout();
    QLabel* label = new QLabel();

    int id = QFontDatabase::addApplicationFont("fonts/Roboto-Bold.ttf");
    QString robotoHeader = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont f(robotoHeader, 15);

    label->setText(header);
    label->setFont(f);

    auto dockContent = new QWidget();
    dockContent->setStyleSheet("background-color:#3C3C3C;border-radius:18px;");
    dockContent->setLayout(dockLayout);
    dockLayout->addWidget(label);
    dockLayout->addWidget(content);

    return dockContent;
}

// deletes any generated keyframes and states on application exit
void strangevismoviemaker::closeEvent(QCloseEvent* event)
{
    QDirIterator it("./snapshots/", { "*.png" });

    while (it.hasNext())
        QFile(it.next()).remove();

    QDirIterator at("./states/", { "*.txt" });

    while (at.hasNext())
        QFile(at.next()).remove();

}
