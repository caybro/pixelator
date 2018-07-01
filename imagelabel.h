#pragma once

#include <QLabel>

class ImageLabel: public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent=nullptr);
    ~ImageLabel() override = default;

    void setImage(const QImage &img);

protected:
    void mousePressEvent(QMouseEvent *ev) override;

signals:
    void pixelClicked(int x, int y, const QColor &color);

private:
    QImage m_image;
};
