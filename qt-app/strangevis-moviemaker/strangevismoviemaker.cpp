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
    m_renderer->setKeyframes(keyframeWrapper, square);
}

void strangevismoviemaker::appendDockWidgets()
{
    qDebug() << "Volume contains values: " << !m_renderer->getVolume()->getDataset().isEmpty();

    auto dockLayout = new QVBoxLayout();
    auto dockContentWrapper = new QWidget();
    dockContentWrapper->setLayout(dockLayout);
    dockContentWrapper->setStyleSheet("background-color: #6D6D6D;");

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
    square = new QSize(QDesktopWidget().availableGeometry().width() * 0.15, QDesktopWidget().availableGeometry().width() * 0.15);
    keyframeWrapper = m_renderer->setKeyframes(new QWidget(), square);
    m_renderer->setKeyframeWrapper(keyframeWrapper);
    keyframes->setWidget(keyframeWrapper);
    keyframes->setMaximumWidth(QDesktopWidget().availableGeometry().width() * 0.15);
    this->addDockWidget(Qt::LeftDockWidgetArea, keyframes);
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
    dockContent->setStyleSheet("background-color: #3C3C3C;border-radius:18px;");
    dockContent->setLayout(dockLayout);
    dockLayout->addWidget(label);
    dockLayout->addWidget(content);

    return dockContent;
}

// deletes any generated keyframes and states on application exit
void strangevismoviemaker::closeEvent(QCloseEvent* event)
{
    QDirIterator it("./states/", { "*.png" });

    while (it.hasNext())
        QFile(it.next()).remove();

    QDirIterator at("./states/", { "*.txt" });

    while (at.hasNext())
        QFile(at.next()).remove();

}
