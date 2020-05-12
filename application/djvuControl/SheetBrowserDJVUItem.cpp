#include "SheetBrowserDJVUItem.h"
#include "document/model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

SheetBrowserDJVUItem::SheetBrowserDJVUItem(deepin_reader::Page *page) : QGraphicsItem()
{
    m_page = page;
}

SheetBrowserDJVUItem::~SheetBrowserDJVUItem()
{
    if (nullptr != m_page)
        delete m_page;
}

void SheetBrowserDJVUItem::setScale(double scale)
{

}

void SheetBrowserDJVUItem::setRotation(Dr::Rotation rotation)
{

}

QRectF SheetBrowserDJVUItem::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, (double)m_page->size().height() * m_scaleFactor, (double)m_page->size().width() * m_scaleFactor);
    default: break;
    }

    return QRectF(0, 0, (double)m_page->size().width() * m_scaleFactor, (double)m_page->size().height() * m_scaleFactor);
}

void SheetBrowserDJVUItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(option->rect, m_image);

    if (m_image.isNull() || m_imageScaleFactor != m_scaleFactor) {
        render(m_scaleFactor, m_rotation, false);
    }
}

void SheetBrowserDJVUItem::render(double scaleFactor, Dr::Rotation rotation, bool readerLater)
{
    if (nullptr == m_page)
        return;

    if (!m_image.isNull() && qFuzzyCompare(scaleFactor, m_scaleFactor) && rotation == m_rotation)
        return;

    m_scaleFactor = scaleFactor;
    m_rotation  = rotation;

    if (!readerLater) {
        m_imageScaleFactor = m_scaleFactor;
        m_image = m_page->render(72, 72, m_rotation, m_scaleFactor);
    }

    update();
}
