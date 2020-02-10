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
#include "PagingWidget.h"

#include "docview/docummentproxy.h"

PagingWidget::PagingWidget(CustomWidget *parent)
    : CustomWidget(QString("PagingWidget"), parent)
{
//    resize(LEFTNORMALWIDTH, 56);

    initWidget();
    initConnections();
    slotUpdateTheme();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

PagingWidget::~PagingWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

/**
 * @brief PagingWidget::initWidget
 * 初始化界面
 */
void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new CustomClickLabel(QString("/xxx"), this);
//    m_pTotalPagesLab->setMinimumWidth(60);
//    m_pTotalPagesLab->setFixedHeight(40);
    DFontSizeManager::instance()->bind(m_pTotalPagesLab, DFontSizeManager::T6);
    m_pTotalPagesLab->setForegroundRole(DPalette::Text);

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setFixedSize(QSize(36, 36));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePage()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(36, 36));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPage()));

//    m_pJumpPageSpinBox = new DSpinBox(this);
//    m_pJumpPageSpinBox->setMinimum(1);
//    m_pJumpPageSpinBox->setValue(1);
//    m_pJumpPageSpinBox->setMinimumWidth(70);
//    m_pJumpPageSpinBox->setFixedHeight(40);
//    m_pJumpPageSpinBox->installEventFilter(this);
//    m_pJumpPageSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//    DFontSizeManager::instance()->bind(m_pJumpPageSpinBox, DFontSizeManager::T6);
//    m_pJumpPageSpinBox->setForegroundRole(DPalette::Text);
    //DLineEdit *m_pJumpPageLineEdit
    m_pJumpPageLineEdit = new DLineEdit(this);
    m_pJumpPageLineEdit->setFixedSize(60, 36);
    m_pJumpPageLineEdit->installEventFilter(this);
    m_pJumpPageLineEdit->setClearButtonEnabled(false);
    DFontSizeManager::instance()->bind(m_pJumpPageLineEdit, DFontSizeManager::T6);
    m_pJumpPageLineEdit->setForegroundRole(DPalette::Text);

    auto hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(10, 0, 10, 0);
    hLayout->addWidget(m_pJumpPageLineEdit);
    hLayout->addSpacing(5);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addStretch(1);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);

    this->setLayout(hLayout);
}

/**
 * @brief PagingWidget::eventFilter
 * 输入框响应回车事件
 * @param watched
 * @param event
 * @return
 */
bool PagingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pJumpPageLineEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            //过滤掉零开头的输入
            if (keyEvent->key() == Qt::Key_0 && m_pJumpPageLineEdit->text().isEmpty()) {
                return true;
            }

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                int index = m_pJumpPageLineEdit->text().trimmed().toInt() - 1;
                if (index >= 0 && index < m_nMaxPage)
                    notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(index));
            }
        } else if (event->type() == QEvent::KeyRelease &&
                   qobject_cast<DLineEdit *>(watched) == m_pJumpPageLineEdit) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_0) {
                QString strvalue = m_pJumpPageLineEdit->text();
                if (strvalue.startsWith("0")) {
                    strvalue = strvalue.right(strvalue.length() - 1);
                    if (strvalue.isEmpty()) {
                        // strvalue=QString("1");
                        m_pJumpPageLineEdit->clear();
                        return true;
                    }
                    m_pJumpPageLineEdit->setText(strvalue);
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PagingWidget::initConnections()
{
    connect(this, SIGNAL(sigDocFilePageChange(const QString &)), SLOT(SlotDocFilePageChange(const QString &)));
    connect(this, SIGNAL(sigDocFileOpenOk()), SLOT(SlotDocFileOpenOk()));

    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

/**
 * @brief PagingWidget::dealWithData
 * 处理全局消息接口
 * @param msgType
 * @return
 */
int PagingWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FILE_PAGE_CHANGE) {                  //  文档页变化了
        emit sigDocFilePageChange(msgContent);
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {     //  颜色主题切换
        emit sigUpdateTheme();
    } else if (msgType == MSG_OPERATION_OPEN_FILE_OK) {     //  文档打开成功了
        emit sigDocFileOpenOk();
    }

    return 0;
}

void PagingWidget::slotUpdateTheme()
{
    if (m_pTotalPagesLab) {
        m_pTotalPagesLab->setForegroundRole(DPalette::Text);
    }
//    if (m_pJumpPageSpinBox) {
//        m_pJumpPageSpinBox->setForegroundRole(DPalette::Text);
//    }
}

void PagingWidget::SlotDocFilePageChange(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        int totalPage = _proxy->getPageSNum();

        int inputData = msgContent.toInt();

        int currntPage = inputData + 1;
        if (currntPage == 1) {
            m_pPrePageBtn->setEnabled(false);
            m_pNextPageBtn->setEnabled(true);
        } else if (currntPage == totalPage) {
            m_pPrePageBtn->setEnabled(true);
            m_pNextPageBtn->setEnabled(false);
        } else {
            m_pPrePageBtn->setEnabled(true);
            m_pNextPageBtn->setEnabled(true);
        }

        m_pJumpPageLineEdit->setText(QString::number(currntPage));
    }
}

//  文档打开成功, 设置总页数 和 当前页码
void PagingWidget::SlotDocFileOpenOk()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        int totalPage = _proxy->getPageSNum();

        m_pTotalPagesLab->setText(QString("/%1").arg(totalPage));
//        m_pJumpPageSpinBox->setMaximum(totalPage);
        m_nMaxPage = totalPage;

        int nCurPage = _proxy->currentPageNo();
        if (nCurPage == 0)  //  已经是第一页了
            m_pPrePageBtn->setEnabled(false);
        else if (nCurPage == totalPage) {   //  已经是最后一页了
            m_pNextPageBtn->setEnabled(false);
        }

        SlotDocFilePageChange(QString::number(nCurPage));
    }
}
//  按钮点击 上一页
void PagingWidget::slotPrePage()
{
    notifyMsg(MSG_OPERATION_PREV_PAGE);
}

//  按钮点击 下一页
void PagingWidget::slotNextPage()
{
    notifyMsg(MSG_OPERATION_NEXT_PAGE);
}
