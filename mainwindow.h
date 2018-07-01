#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void changeEvent(QEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onOpenFile();
    void onSaveFile();
    void onPixelClicked(int x, int y, const QColor &color);
    void onApplyClicked();

private:
    bool loadFile(const QString &fileName);
    void setImage(const QImage &newImage);
    Ui::MainWindow *m_ui{nullptr};
    QImage m_image;
    QPoint m_lastPos;
    QColor m_lastColor;
};
