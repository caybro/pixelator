#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QPixmap>
#include <QMouseEvent>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    connect(m_ui->imageLabel, &ImageLabel::pixelClicked, this, &MainWindow::onPixelClicked);

    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(m_ui->spRed, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        m_ui->btnApply->setEnabled(true);
    });
    connect(m_ui->spGreen, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        m_ui->btnApply->setEnabled(true);
    });
    connect(m_ui->spBlue, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        m_ui->btnApply->setEnabled(true);
    });
    connect(m_ui->spAlpha, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        m_ui->btnApply->setEnabled(true);
    });

    connect(m_ui->btnApply, &QPushButton::clicked, this, &MainWindow::onApplyClicked);
    connect(m_ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for(const QByteArray &mimeTypeName: supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!isWindowModified()) {
        event->accept();
        return;
    }

    auto ret = QMessageBox::question(this, tr("Picture Modified"), tr("Do you want to save the modified picture?"),
                                     QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        onSaveFile();
    }
    event->setAccepted(ret != QMessageBox::Cancel);
}

void MainWindow::onOpenFile()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::onPixelClicked(int x, int y, const QColor &color)
{
    qInfo() << "!!! Pixel clicked:" << x << y << color;
    m_ui->groupBox->setEnabled(true);
    m_lastPos = QPoint(x, y);
    m_lastColor = color;
    m_ui->labelColor->setText(color.name());
    m_ui->labelColor->setStyleSheet(QStringLiteral("QLabel {background-color: '%1' }").arg(color.name()));
    m_ui->spRed->setValue(color.red());
    m_ui->spGreen->setValue(color.green());
    m_ui->spBlue->setValue(color.blue());
    m_ui->spAlpha->setValue(color.alpha());
}

void MainWindow::onApplyClicked()
{
    if (!m_lastPos.isNull()) {
        QColor newColor(m_ui->spRed->value(), m_ui->spGreen->value(), m_ui->spBlue->value(), m_ui->spAlpha->value());
        m_image.setPixelColor(m_lastPos, newColor);
        qInfo() << "!!! New pixel color" << m_lastPos << newColor;
        m_ui->imageLabel->setImage(m_image);
        m_ui->actionSave->setEnabled(true);
        setWindowModified(true);
    }
}

void MainWindow::onSaveFile()
{
    m_image.save(windowFilePath());
    m_ui->actionSave->setEnabled(false);
    setWindowModified(false);
}

bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        m_ui->groupBox->setEnabled(false);
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);
    setWindowTitle(QStringLiteral("%1[*] - %2").arg(windowFilePath()).arg("Pixelator"));

    const QString message = tr("\"%1\", %2x%3, depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(m_image.width()).arg(m_image.height()).arg(m_image.depth());
    statusBar()->showMessage(message);

    m_ui->actionSave->setEnabled(false);
    m_lastPos = QPoint();
    m_lastColor = QColor();
    m_ui->groupBox->setEnabled(false);
    m_ui->labelColor->clear();
    m_ui->labelColor->setStyleSheet(QString());
    m_ui->spRed->clear();
    m_ui->spGreen->clear();
    m_ui->spBlue->clear();
    m_ui->spAlpha->clear();

    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    m_image = newImage;
    m_ui->imageLabel->setImage(m_image);
}
