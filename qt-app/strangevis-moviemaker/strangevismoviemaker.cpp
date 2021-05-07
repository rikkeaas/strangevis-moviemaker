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
    
    animationMenu = ui.menuBar->addMenu("Animation");
    cutMenu = ui.menuBar->addMenu("Cut");

    QAction* fileOpenAction = new QAction("Open", this);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    ui.menuFile->addAction(fileOpenAction);

    QAction* highresScreenshotAction = new QAction("Screenshot", this);
    connect(highresScreenshotAction, SIGNAL(triggered()), this, SLOT(highresScreenshot()));
    ui.menuFile->addAction(highresScreenshotAction);

    QAction* setBackgroundColorAction = new QAction("Choose Background Color", this);
    connect(setBackgroundColorAction, SIGNAL(triggered()), this, SLOT(setBackgroundColor()));
    ui.menuEdit->addAction(setBackgroundColorAction);

    QAction* raySamplingDistance = new QAction("Set ray sampling distance multiplier", this);
    connect(raySamplingDistance, SIGNAL(triggered()), this, SLOT(raySamplingDistance()));
    ui.menuEdit->addAction(raySamplingDistance);

    QAction* selectHistogramYScaling = new QAction("Choose histogram Y-axis scaling", this);
    connect(selectHistogramYScaling, SIGNAL(triggered()), this, SLOT(selectHistogramYScaling()));
    ui.menuEdit->addAction(selectHistogramYScaling);

    QAction* toggleLightVolumeTransformation = new QAction("Toggle light/volume transformation", this);
    connect(toggleLightVolumeTransformation, SIGNAL(triggered()), m_renderer, SLOT(toggleLightVolumeTransformation()));
    ui.menuEdit->addAction(toggleLightVolumeTransformation);

    QAction* typeOfAnimation = new QAction("Set type of interpolation", this);
    connect(typeOfAnimation, SIGNAL(triggered()), this, SLOT(setTypeOfAnimation()));
    animationMenu->addAction(typeOfAnimation);

    QAction* animationTimerAction = new QAction("Adjust Animation Duration", this);
    connect(animationTimerAction, SIGNAL(triggered()), this, SLOT(adjustAnimationDuration()));
    animationMenu->addAction(animationTimerAction);

    QAction* clearStatesAction = new QAction("Clear All States", this);
    connect(clearStatesAction, SIGNAL(triggered()), this, SLOT(clearStates()));
    animationMenu->addAction(clearStatesAction);
 
    QAction* cutAction = new QAction("Cut type", this);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    cutMenu->addAction(cutAction);

    QAction* setRadius = new QAction("Set cut radius (spherical cut)", this);
    connect(setRadius, SIGNAL(triggered()), this, SLOT(setRadius()));
    cutMenu->addAction(setRadius);
    cutMenu->actions().at(1)->setEnabled(false);

    QAction* setSize = new QAction("Set cut size (cubical cut)", this);
    connect(setSize, SIGNAL(triggered()), this, SLOT(setCutSize()));
    cutMenu->addAction(setSize);
    cutMenu->actions().at(2)->setEnabled(false);

    QAction* showCut = new QAction("Visualize cut geometry", this);
    connect(showCut, SIGNAL(triggered()), this, SLOT(setShowCut()));
    cutMenu->addAction(showCut);
    cutMenu->actions().at(3)->setEnabled(false);

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

void strangevismoviemaker::cut()
{
    QStringList items;
    items << "None" << "Spherical" << "Cubical";
    QString item = QInputDialog::getItem(0, "Type of cut", "Type of cut: ", items, m_cutType, false, nullptr, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));
    if (!item.isEmpty())
    {
        m_cutType = items.indexOf(item);
        if (m_cutType == 0)
        {
            m_renderer->setCubeCut(false);
            m_renderer->setSphereCut(false);
            cutMenu->actions().at(1)->setEnabled(false);
            cutMenu->actions().at(2)->setEnabled(false);
            cutMenu->actions().at(3)->setEnabled(false);

        }
        if (m_cutType == 1)
        {
            m_renderer->setCubeCut(false);
            m_renderer->setSphereCut(true);
            cutMenu->actions().at(1)->setEnabled(true);
            cutMenu->actions().at(2)->setEnabled(false);
            cutMenu->actions().at(3)->setEnabled(true);
            m_renderer->setSphereRadius(m_cutRadius);
        }
        if (m_cutType == 2)
        {
            m_renderer->setCubeCut(true);
            m_renderer->setSphereCut(false);
            cutMenu->actions().at(1)->setEnabled(false);
            cutMenu->actions().at(2)->setEnabled(true);
            cutMenu->actions().at(3)->setEnabled(true);
            m_renderer->setCubeSize(m_cutSize);
        }
    }
}

void strangevismoviemaker::setShowCut()
{
    QStringList items;
    items << "Don't show cut" << "Show cut in volume" << "Show cut in front";
    QString item = QInputDialog::getItem(0, "Visualize Cut Geometry", "Select cut visualization", items, m_showCut, false, nullptr, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));

    if (!item.isEmpty())
    {
        m_showCut = items.indexOf(item);
        
        if (m_showCut == 0)
        {
            m_renderer->setShowCut(false, false);
        }
        if (m_showCut == 1)
        {
            m_renderer->setShowCut(true, false);
        }
        if (m_showCut == 2)
        {
            m_renderer->setShowCut(true, true);
        }
    }
}

void strangevismoviemaker::setTypeOfAnimation()
{
    QStringList items;
    items << "Linear" << "Catmull Rom";
    QString item = QInputDialog::getItem(0, "Set type of interpolation", "Select type of interpolation", items, m_animationType, false, nullptr, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));

    if (!item.isEmpty())
    {
        m_animationType = items.indexOf(item);

        if (m_animationType == 0)
        {
            m_renderer->setInterpolationType(false);
        }
        else
        {
            m_renderer->setInterpolationType(true);
        }
    }
}

void strangevismoviemaker::setRadius()
{
    bool ok;
    double radius = QInputDialog::getDouble(0, "Cut Sphere Radius", "Set radius of cut: ", m_cutRadius, 0.0, 2.0, 2, &ok, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));
    if (ok)
    {
        m_cutRadius = radius;
        m_renderer->setSphereRadius(radius);
    }
}

void strangevismoviemaker::setCutSize()
{
    bool ok;
    double size = QInputDialog::getDouble(0, "Cut Cube Size", "Set size of cut: ", m_cutSize, 0.0, 2.0, 2, &ok, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));
    if (ok)
    {
        m_cutSize = size;
        m_renderer->setCubeSize(size);
    }
}

void strangevismoviemaker::adjustAnimationDuration()
{
    auto duration = QInputDialog::getDouble(0, "Animation Speed",
        "Set Animation Speed in Seconds:", m_renderer->getAnimationDuration(), 0.3, 5.0, 1, nullptr, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));
    m_renderer->setAnimationDuration(duration);
}

void strangevismoviemaker::appendDockWidgets()
{
    auto dockLayout = new QVBoxLayout();
    auto dockContentWrapper = new QWidget();
    dockContentWrapper->setLayout(dockLayout);
    dockContentWrapper->setStyleSheet("background-color: #6D6D6D;");

    QDockWidget* toolbox = new QDockWidget(tr("Toolbox"), this);
    Histogram* h = new Histogram(m_renderer, true);
    QObject::connect(m_renderer, &Renderer::updateLayers, h->m_layerHandler, &LayerHandler::setLayers);
    QObject::connect(this, &strangevismoviemaker::updateHistogramYScaling, h, &Histogram::updateHistogramYScaling);
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

void strangevismoviemaker::raySamplingDistance()
{
    bool ok;
    auto rsdm = QInputDialog::getDouble(0, "Ray sampling distance multiplier",
        "Set ray sampling distance multiplier:", m_renderer->getRaySamplingDistance(), 0.5, 100.0, 1, &ok, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));
    m_renderer->setRaySamplingDistance(rsdm);
}

void strangevismoviemaker::clearStates()
{
    m_renderer->clearStates();
}

void strangevismoviemaker::selectHistogramYScaling()
{
    qDebug() << "Hello";
    QStringList items;
    items << "Linear" << "Logarithmic (base 10)";
    QString item = QInputDialog::getItem(0, "Set scaling of Y-axis in histogram", "Select Y-axis scaling", items, m_histogramYScaling, false, nullptr, (windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint));

    if (!item.isEmpty())
    {
        m_histogramYScaling = items.indexOf(item);

        if (m_histogramYScaling == 0)
        {
            updateHistogramYScaling(false);
        }
        else
        {
            updateHistogramYScaling(true);
        }
    }
}