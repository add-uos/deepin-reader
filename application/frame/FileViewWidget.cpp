#include "FileViewWidget.h"
#include <QGridLayout>
#include <QClipboard>
#include "controller/DataManager.h"
#include "application.h"

#include "docview/docummentproxy.h"
#include <QPrinter>
#include <QPrintDialog>
#include <DMessageBox>
#include <QPrintPreviewDialog>
#include "translator/Frame.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
{
    setMouseTracking(true); //  接受 鼠标滑动事件
    setContextMenuPolicy(Qt::CustomContextMenu);

    initWidget();
    initConnections();
}

FileViewWidget::~FileViewWidget()
{
//    if (m_pDocummentProxy) {
//        m_pDocummentProxy->closeFile();
//        m_pDocummentProxy->waitThreadAndClearEnd();
//    }
}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    m_pDocummentProxy = DocummentProxy::instance(this);

    m_pDocummentFileHelper = new DocummentFileHelper(this);
}

//  鼠标双击事件
void FileViewWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    CustomWidget::mouseDoubleClickEvent(event);
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    auto pDocummentProxy = DocummentProxy::instance();
    if (pDocummentProxy) {
        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = pDocummentProxy->global2RelativePoint(globalPos);
        if (m_nCurrentHandelState == Handel_State) {    //   手型状态下， 按住鼠标左键 位置进行移动
            if (m_bSelectOrMove) {
                QPoint mvPoint = m_pHandleMoveStartPoint - globalPos;
                int mvX = mvPoint.x();
                int mvY = mvPoint.y();

                pDocummentProxy->pageMove(mvX, mvY);

                m_pHandleMoveStartPoint = globalPos;
            }
        } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
            pDocummentProxy->showMagnifier(docGlobalPos);
        } else {
            if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
                m_pMoveEndPoint = docGlobalPos;
                pDocummentProxy->mouseSelectText(m_pStartPoint, m_pMoveEndPoint);
            } else {
                //  首先判断文档划过属性
                Page::Link *pLink  = pDocummentProxy->mouseBeOverLink(docGlobalPos);
                if (pLink) {
                    setCursor(QCursor(Qt::PointingHandCursor));
                } else {
                    if (pDocummentProxy->mouseBeOverText(docGlobalPos))
                        setCursor(QCursor(Qt::IBeamCursor));
                    else {
                        setCursor(QCursor(Qt::ArrowCursor));
                    }
                }
            }
        }
    }
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    Qt::MouseButton nBtn = event->button();
    QPoint globalPos = event->globalPos();
    if (nBtn == Qt::LeftButton) {
        auto pDocummentProxy = DocummentProxy::instance();
        QPoint docGlobalPos = pDocummentProxy->global2RelativePoint(globalPos);

        //  点击的时候　先判断　点击处　　是否有链接之类
        Page::Link *pLink  = pDocummentProxy->mouseBeOverLink(docGlobalPos);
        if (pLink) {
            m_pDocummentFileHelper->onClickPageLink(pLink);
        } else {
            m_bSelectOrMove = true;

            if (m_nCurrentHandelState == Handel_State) {
                m_pHandleMoveStartPoint = globalPos;     //  变成手，　需要的是　相对坐标
            } else if (m_nCurrentHandelState == Default_State) {
                pDocummentProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮
                m_pStartPoint = docGlobalPos;
            }
        }

        m_nPage = pDocummentProxy->pointInWhichPage(m_pStartPoint);

//        qDebug() << "pointInWhichPage page:" << m_nPage;

        // 判断鼠标点击的地方是否有高亮
        QString selectText,t_strContant;

        m_bIsHighLight = pDocummentProxy->annotationClicked(docGlobalPos, selectText, m_strUUid);

        t_strContant.clear();
        t_strContant = m_strUUid.trimmed() + QString("%") + QString::number((m_bIsHighLight?1:0)) + QString("%") + QString::number(m_nPage);
        sendMsg(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, t_strContant);
    }
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    m_bSelectOrMove = false;
    CustomWidget::mouseReleaseEvent(event);
}

void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    slotSetWidgetAdapt();

    CustomWidget::resizeEvent(event);
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;

    auto pDocummentProxy = DocummentProxy::instance();
    if (pDocummentProxy) {
        QString sSelectText =  "";
        pDocummentProxy->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

        QPoint tempPoint = this->mapToGlobal(point);
        m_pRightClickPoint = pDocummentProxy->global2RelativePoint(tempPoint);

        m_nPage = pDocummentProxy->pointInWhichPage(m_pRightClickPoint);

        QString sAnnotationText = "",struuid("");
        m_bIsHighLight = pDocummentProxy->annotationClicked(m_pRightClickPoint, sAnnotationText,struuid);

        if (sSelectText != "" || m_bIsHighLight) {
            //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
            if (m_pTextOperationWidget == nullptr) {
                m_pTextOperationWidget = new TextOperationWidget(this);
            }
            m_pTextOperationWidget->showWidget(tempPoint.x(), tempPoint.y(), m_bIsHighLight, sSelectText);
        } else {
            if (m_pDefaultOperationWidget == nullptr) {
                m_pDefaultOperationWidget = new DefaultOperationWidget(this);
            }
            m_pDefaultOperationWidget->showWidget(tempPoint.x(), tempPoint.y());
        }
    }
}

//  放大镜　控制
void FileViewWidget::slotMagnifying(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) {
        m_nCurrentHandelState = Magnifier_State;
        this->setCursor(Qt::BlankCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
        DocummentProxy::instance()->closeMagnifier();
    }
}

//  手势控制
void FileViewWidget::slotSetHandShape(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) { //  手形
        m_nCurrentHandelState = Handel_State;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
    }

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    DocummentProxy::instance()->mouseSelectTextClear();
}

//  添加高亮颜色
void FileViewWidget::slotFileAddAnnotation(const QString &sColor)
{
    DataManager::instance()->setBIsUpdate(true);
    QList<QColor> colorList = {};

    m_strUUid = DocummentProxy::instance()->addAnnotation(m_pRightClickPoint, m_pRightClickPoint);
}

//  移除高亮, 有注释 则删除注释
void FileViewWidget::slotFileRemoveAnnotation()
{
    DataManager::instance()->setBIsUpdate(true);
    QString sUuid = DocummentProxy::instance()->removeAnnotation(m_pRightClickPoint);

    sendMsg(MSG_NOTE_DLTNOTEITEM, sUuid);

    DocummentProxy::instance()->removeAnnotation(sUuid);
}

void FileViewWidget::slotFileAddNote(const QString &note)
{
    if(!m_bIsHighLight){
        // 添加高亮
        slotFileAddAnnotation(QString(""));
    }

    QString t_str = m_strUUid.trimmed() + QString("%") + note.trimmed() + QString("%%1").arg(m_nPage);

    //send to note list widget on the left
    sendMsg(MSG_NOTE_ADDITEM, t_str);

    auto proxy = DocummentProxy::instance();
    proxy->setAnnotationText(m_nPage, m_strUUid, note);
//    qDebug() << "setAnnotationText page:" << m_nPage << " uuid:" << m_strUUid << " note:" << note;
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigSetHandShape(const QString &)), this, SLOT(slotSetHandShape(const QString &)));
    connect(this, SIGNAL(sigMagnifying(const QString &)), this, SLOT(slotMagnifying(const QString &)));
    connect(this, SIGNAL(sigWidgetAdapt()), this, SLOT(slotSetWidgetAdapt()));
    connect(this, SIGNAL(sigPrintFile()), this, SLOT(slotPrintFile()));

    connect(this, SIGNAL(sigFileAddAnnotation(const QString &)), this, SLOT(slotFileAddAnnotation(const QString &)));
    connect(this, SIGNAL(sigFileRemoveAnnotation()), this, SLOT(slotFileRemoveAnnotation()));

    connect(this, SIGNAL(sigFileAddNote(const QString &)), this, SLOT(slotFileAddNote(const QString &)));
}

//  打印
void FileViewWidget::slotPrintFile()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);

    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [=] (QPrinter *printer) {
//        currentWrapper()->textEditor()->print(printer);


    });
//    DTextEdit *wrapper = currentWrapper()->textEditor();
//    const QString &filePath = wrapper->filepath;
//    const QString &fileDir = QFileInfo(filePath).dir().absolutePath();

//    if (fileDir == m_blankFileDir) {
//        printer.setOutputFileName(QString("%1/%2.pdf").arg(QDir::homePath(), m_tabbar->currentName()));
//    } else {
//        printer.setOutputFileName(QString("%1/%2.pdf").arg(fileDir, QFileInfo(filePath).baseName()));
//    }

//    printer.setOutputFormat(QPrinter::PdfFormat);

//    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [=] (QPrinter *printer) {
//        currentWrapper()->textEditor()->print(printer);
//    });

    preview.exec();
//    QPrinter printer;
//    QString printerName = printer.printerName();
//    if ( 1||printerName.size() > 0) {
//        QPrintDialog printDialog(&printer, this);
//        if (printDialog.exec() == QDialog::Accepted) {
//            //  print

//        }
//    } else {
//        DMessageBox::warning(nullptr, "", Frame::sPrintErrorNoDevice);
//    }
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::slotSetWidgetAdapt()
{
    if (m_nAdapteState == WIDGET_State) {
        int nWidth = this->width();
        DocummentProxy::instance()->adaptWidthAndShow(nWidth);
    } else if (m_nAdapteState == HEIGHT_State) {
        int nHeight = this->height();
        DocummentProxy::instance()->adaptHeightAndShow(nHeight);
    }
}

//  标题栏的菜单消息处理
int FileViewWidget::dealWithTitleRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_MAGNIFYING:            //  放大镜信号
        emit sigMagnifying(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_HANDLESHAPE:           //  手势 信号
        emit sigSetHandShape(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_SELF_ADAPTE_HEIGHT:    //  自适应　高度
        if (msgContent == "1") {
            m_nAdapteState = HEIGHT_State ;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_SELF_ADAPTE_WIDTH:    //   自适应宽度
        if (msgContent == "1") {
            m_nAdapteState = WIDGET_State;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PRINT :      //  打印
        emit sigPrintFile();
        return  ConstantMsg::g_effective_res;
    case MSG_FILE_ROTATE:           //  文档旋转了
        emit sigWidgetAdapt();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  文档右键菜单请求处理
int FileViewWidget::dealWithFileMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_TEXT_ADD_HIGHLIGHTED:    //  高亮显示
        emit sigFileAddAnnotation(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED: //  移除高亮显示
        emit sigFileRemoveAnnotation();
        return ConstantMsg::g_effective_res;
    case MSG_NOTE_ADDCONTANT:                      //  添加注释
        emit sigFileAddNote(msgContent);
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = dealWithTitleRequest(msgType, msgContent);
    if (nRes != ConstantMsg::g_effective_res) {

        nRes = dealWithFileMenuRequest(msgType, msgContent);
        if (nRes != ConstantMsg::g_effective_res) {

        }
    }

    return 0;
}
