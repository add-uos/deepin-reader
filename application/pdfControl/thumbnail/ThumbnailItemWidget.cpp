/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ThumbnailItemWidget.h"

#include <QVBoxLayout>

#include "business/AppInfo.h"

ThumbnailItemWidget::ThumbnailItemWidget(DWidget *parent)
    : CustomItemWidget(THUMBAIL_ITEM_WIDGET, parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    initWidget();
}

// 处理消息接口
int ThumbnailItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

// 是否被选中，选中就就填充颜色
void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }

    if (m_pPageNumber) {
        m_pPageNumber->setSelect(paint);
    }
}

void ThumbnailItemWidget::rotateThumbnail(int angle)
{
    auto imageLabel = this->findChild<ImageLabel *>();
    if (imageLabel) {
        imageLabel->setRotateAngle(angle);
        if (imageLabel->hasThumbnail()) {
            imageLabel->rotateImage();
        }
    }
}

void ThumbnailItemWidget::qSetBookMarkShowStatus(const bool &bshow)
{
    m_pPicture->setBookMarkStatus(bshow);
    m_pPicture->update();
}

// 初始化界面
void ThumbnailItemWidget::initWidget()
{
    m_pPageNumber = new PageNumberLabel(this);
    int tW = 146;
    int tH = 18;
    dApp->adaptScreenView(tW, tH);
    m_pPageNumber->setMinimumSize(QSize(tW, tH));
    m_pPageNumber->setAlignment(Qt::AlignCenter);

    m_pPicture = new ImageLabel(this);
    tW = 146;
    tH = 174;
    dApp->adaptScreenView(tW, tH);
    m_pPicture->setFixedSize(QSize(tW, tH));
    m_pPicture->setSize(QSize(tW, tH));
    m_pPicture->setAlignment(Qt::AlignCenter);
    m_pPicture->setRadius(ICON_BIG);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(0, 0, 0, 0);
    t_vLayout->setSpacing(0);

    t_vLayout->addWidget(m_pPicture);
    t_vLayout->addWidget(m_pPageNumber);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 32, 0);
    t_hLayout->addStretch(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addStretch(1);

    this->setLayout(t_hLayout);
}
/**
 * @brief ThumbnailItemWidget::adaptWindowSize
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void ThumbnailItemWidget::adaptWindowSize(const double &scale)
{
//    double width = 1.0;
//    double height = 1.0;

//    width = static_cast<double>(this->width()) * scale;
//    height = static_cast<double>(this->height()) * scale;

//    this->resize(QSize(static_cast<int>(width), static_cast<int>(height)));

    if (m_pPicture) {
        m_pPicture->scaleImage(scale);
    }
}

