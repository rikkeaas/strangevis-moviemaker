#include "strangevismoviemaker.h"
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
#include <QDateTime>


strangevismoviemaker::strangevismoviemaker(Renderer* renderer, QWidget *parent)
    : QMainWindow(parent)
{
    renderer->setParent(this);
    m_renderer = renderer;

    ui.setupUi(this);
       
    QAction* fileOpenAction = new QAction("Open", this);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    ui.menuFile->addAction(fileOpenAction);

    QAction* highresScreenshotAction = new QAction("Screenshot", this);
    connect(highresScreenshotAction, SIGNAL(triggered()), this, SLOT(highresScreenshot()));
    ui.menuFile->addAction(highresScreenshotAction);

    QAction* setBackgroundColorAction = new QAction("Choose Background Color", this);
    connect(setBackgroundColorAction, SIGNAL(triggered()), this, SLOT(setBackgroundColor()));
    ui.menuEdit->addAction(setBackgroundColorAction);

    QAction* animationTimerAction = new QAction("Adjust Animation Duration", this);
    connect(animationTimerAction, SIGNAL(triggered()), this, SLOT(adjustAnimationDuration()));
    ui.menuEdit->addAction(animationTimerAction);

    QAction* clearStatesAction = new QAction("Clear All States", this);
    connect(clearStatesAction, SIGNAL(triggered()), this, SLOT(clearStates()));
    ui.menuEdit->addAction(clearStatesAction);

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
            for (QDockWidget* dw : this->findChildren<QDockWidget *>())
            {
                this->removeDockWidget(dw);
            }
            appendDockWidgets();
            qDebug() << "Loaded volume " << fileName; 
        } else {
            qDebug() << "Failed to load volume " << fileName;
        }
    }
}

void strangevismoviemaker::highresScreenshot()
{
    if (!QDir("screenshots").exists()) {
        QDir().mkdir("screenshots");
    }
    QTimeEdit* timeEdit = new QTimeEdit();
    QTime currentTime = timeEdit->time().currentTime();
    currentTime.toString().replace(":", "");
    QString filename = "screenshots/screenshot";
    filename.append(currentTime.toString().replace(":", ""));
    filename.append(".png");
    QString f = QString(filename);
    m_renderer->grab().save(f);
}

void strangevismoviemaker::setBackgroundColor()
{
    m_renderer->setBackgroundColor();
}

void strangevismoviemaker::adjustAnimationDuration()
{
    auto duration = QInputDialog::getDouble(0, "Animation Speed",
        "Set Animation Speed in Seconds:", m_renderer->getAnimationDuration(), 0.3, 5.0);
    m_renderer->setAnimationDuration(duration);
}

void strangevismoviemaker::appendDockWidgets()
{
    auto dockLayout = new QVBoxLayout();
    auto dockContentWrapper = new QWidget();
    dockContentWrapper->setLayout(dockLayout);
    dockContentWrapper->setStyleSheet("background-color: #6D6D6D;");

    QDockWidget* toolbox = new QDockWidget(tr("Toolbox"), this);
    Histogram* h = new Histogram(m_renderer);
    QObject::connect(m_renderer, &Renderer::updateLayers, h->m_layerHandler, &LayerHandler::setLayers);
    dockLayout->addWidget(toolbarContent(h, QString("Layers")));

    dockContentWrapper->setLayout(dockLayout);
    toolbox->setWidget(dockContentWrapper);
    formatDockWidgets(toolbox);
    this->addDockWidget(Qt::LeftDockWidgetArea, toolbox);

    if (!QDir("states").exists()) {
        QDir().mkdir("states");
    }
    QDockWidget* keyframes = new QDockWidget(tr("Keyframe Handler"), this);
    formatDockWidgets(keyframes);
    square = new QSize(QDesktopWidget().availableGeometry().width() * 0.2, QDesktopWidget().availableGeometry().width() * 0.2);
    keyframeWrapper = m_renderer->setKeyframes(new QWidget(), square);
    m_renderer->setKeyframeWrapper(keyframeWrapper);
    keyframes->setWidget(keyframeWrapper);
    keyframes->setMaximumWidth(QDesktopWidget().availableGeometry().width() * 0.2);
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
    bool keepStatesOnClose = true;
    if (!keepStatesOnClose) {
        QDirIterator it("./states/", { "*.png" });

        while (it.hasNext())
            QFile(it.next()).remove();

        QDirIterator at("./states/", { "*.txt" });

        while (at.hasNext())
            QFile(at.next()).remove();
    }
}

void strangevismoviemaker::clearStates()
{
    m_renderer->clearStates();
}