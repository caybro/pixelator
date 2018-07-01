#include <QMouseEvent>
#include <QDebug>

#include "imagelabel.h"

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void ImageLabel::setImage(const QImage &img)
{
    m_image = img;
    setPixmap(QPixmap::fromImage(m_image));
}

void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    const int x = ev->x();
    const int y = ev->y();
    if (!m_image.isNull() && ev->button() == Qt::LeftButton && x < m_image.width() && y < m_image.height()) {
        emit pixelClicked(x, y, m_image.pixelColor(x, y));
        ev->accept();
    }
    QLabel::mousePressEvent(ev);
}
