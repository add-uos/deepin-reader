#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "docview/commonstruct.h"
#include "searchtask.h"
#include <DWidget>
#include <DScrollArea>
#include <DScrollBar>
#include <DGuiApplicationHelper>
#include <QList>
#include <QVBoxLayout>
#include <QPoint>
#include <QColor>
#include <QVideoWidget>
#include <QThread>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

#define     MAXPAGEHEIGHT       20000

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};
#include <QtDebug>

class SearchTask;
class DocummentBase;
class DocummentBasePrivate;
//class ThreadLoadDocumment : public QThread
//{
//    Q_OBJECT
//public:
//    ThreadLoadDocumment();
//    void setDoc(DocummentBase *doc, QString path);
//    void setRestart();
//signals:
//    void signal_docLoaded(bool);
//protected:
//    virtual void run();

//private:
//    DocummentBase *m_doc;
//    QString m_path;
//    bool restart;
//};
class ThreadLoadData : public QThread
{
    Q_OBJECT
public:
    ThreadLoadData();
    void setDoc(DocummentBase *doc);
    void setRestart();
signals:
    void signal_dataLoaded(bool);
protected:
    virtual void run();

private:
    DocummentBase *m_doc;
    bool restart;
};

class MagnifierWidget: public DWidget
{
    Q_OBJECT
public:
    MagnifierWidget(DWidget *parent = nullptr);
    void setPixmap(QPixmap pixmap);
    void setPoint(QPoint point);
    int getMagnifierRadius();
    double getMagnifierScale();
    int getMagnifierRingWidth();
    void setMagnifierRadius(int radius);
    void setMagnifierScale(double scale);
    void setMagnifierRingWidth(int ringWidth);
    void setMagnifierColor(QColor color);
    void stopShow();
    void startShow();
    bool showState();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QColor m_magnifiercolor;
    int m_magnifierradius;
    int m_magnifierringwidth;
    QPoint m_magnifierpoint;
    QPixmap m_magnifierpixmap;
    double m_magnifierscale;
    bool bStartShow;
    int m_magnifiermapradius;
    int m_magnifierringmapwidth;
//    int m_magnifiershadowwidth;
};

class SlidWidget: public DWidget
{
    Q_OBJECT
public:
    SlidWidget(DWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

class DocummentBasePrivate: public QObject
{
    Q_OBJECT
public:
    DocummentBasePrivate(DocummentBase *parent): q_ptr(parent),
        m_pages(),
        m_widgets(),
        m_pagecountsearch()
    {
        m_widget = nullptr;
        m_vboxLayout = nullptr;
        m_magnifierwidget = nullptr;
        m_slidewidget = nullptr;
        pslidelabel = nullptr;
        pslideanimationlabel = nullptr;
        pblankwidget = nullptr;
        m_bModified = false;
        m_bslidemodel = false;
        m_slidepageno = -1;
        m_currentpageno = 0;
        m_scale = 1.0;
        m_maxzoomratio = 5.0;
        m_rotate = RotateType_0;
        donotneedreloaddoc = false;
        m_magnifierpage = -1;
        m_viewmode = ViewMode_SinglePage;
        m_lastmagnifierpagenum = -1;
        animationfirst = nullptr;
        animationsecond = nullptr;
        animationgroup = nullptr;
        m_bsearchfirst = true;
        m_findcurpage = -1;
        m_imagewidth = 0.1;
        m_imageheight = 0.1;
        m_cursearch = 1;
        bcloseing = false;
        m_searchTask = nullptr;
        bfindnext = true;
        m_bScanningcopy = false;
        showslidwaittimer = nullptr;
        qwfather = nullptr;
//        loadpagewaittimer = nullptr;
        m_fileinfo = new stFileInfo;
        bautoplayslide = false;
        autoplayslidtimer = nullptr;
        m_autoplayslidmsec = 0;
        m_bautoplayslidreset = false;

    }

    ~DocummentBasePrivate()
    {
        if (!animationfirst) {
            animationfirst->deleteLater();
            animationfirst = nullptr;
        }
        if (!animationsecond) {
            animationsecond->deleteLater();
            animationsecond = nullptr;
        }
        if (!animationgroup) {
            animationgroup->deleteLater();
            animationgroup = nullptr;
        }
        if (m_searchTask) {
            delete m_searchTask;
            m_searchTask = nullptr;
        }
        if (m_magnifierwidget) {
            m_magnifierwidget->deleteLater();
            m_magnifierwidget = nullptr;

        }
        if (m_slidewidget) {
            m_slidewidget->deleteLater();
            m_slidewidget = nullptr;
        }
        if (nullptr != m_fileinfo) {
            delete m_fileinfo;
            m_fileinfo = nullptr;
        }
    }

    QVector<PageBase *> m_pages;
    QList<DWidget *>m_widgets;
    QList<QRect>m_widgetrects;
    DWidget *m_widget;
//    DocMainWidget *m_widget;
    QVBoxLayout *m_vboxLayout;
    MagnifierWidget *m_magnifierwidget;
    SlidWidget *m_slidewidget;
    DLabel *pslideanimationlabel;
    DLabel *pslidelabel;
    DLabel *pblankwidget;
    ViewMode_EM m_viewmode;
    int m_lastmagnifierpagenum;
    int m_magnifierpage;
    int m_slidepageno;
    int m_currentpageno;
    double m_scale;
    double m_maxzoomratio;//最大放大比例
    mutable bool m_bModified;
    bool m_bslidemodel;
    //    ThreadLoadWords m_threadloadwords;
    RotateType_EM m_rotate;
    bool donotneedreloaddoc;
    //    bool m_wordsbload;
    QPoint m_magnifierpoint;
    QPropertyAnimation *animationfirst;
    QPropertyAnimation *animationsecond;
    QParallelAnimationGroup *animationgroup;
    SearchTask *m_searchTask;
    int m_findcurpage;
    QMap<int, int> m_pagecountsearch; //搜索结果页对应当前页个数
    int m_cursearch;
    bool m_bsearchfirst;
    double m_imagewidth;
    double m_imageheight;
    bool bcloseing;
    bool bfindnext;//上一次搜索结果是向前翻还是向后翻
    bool m_bScanningcopy;//当前打开的是否为扫描件
    QTimer *showslidwaittimer;
    DWidget *qwfather;
//    QTimer *loadpagewaittimer;
    stFileInfo *m_fileinfo;
//    ThreadLoadDocumment threadloaddoc;
    ThreadLoadData threadloaddata;
    bool bautoplayslide;
    QTimer *autoplayslidtimer;
    int m_autoplayslidmsec;
    bool m_bautoplayslidreset;//播放到最后一页重置状态从头可以从头开始播放


signals:
    void signal_docummentLoaded(bool);
protected slots:
    virtual void loadDocumment(QString filepath) {}
protected:
    DocummentBase *q_ptr;
    Q_DECLARE_PUBLIC(DocummentBase)
};


class DocummentBase: public DScrollArea
{
    Q_OBJECT
public:
    DocummentBase(DocummentBasePrivate *ptr = nullptr, DWidget *parent = nullptr);
    ~DocummentBase();
    virtual bool loadDocumment(QString filepath) = 0;
    virtual bool bDocummentExist() = 0;
    virtual bool getImage(int pagenum, QImage &image, double width, double height);
    virtual bool save(const QString &filePath, bool withChanges)
    {
        //        qDebug() << "do nothing";
        return false;
    }
    virtual bool saveas(const QString &filePath, bool withChanges)
    {
        return false;
    }
    virtual QString removeAnnotation(const QPoint &startpos) {}

    virtual void removeAnnotation(const QString &struuid, int ipage = -1) {}
    virtual QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) {}
    virtual void search(const QString &strtext, QColor color = Qt::yellow) {}
    virtual void getAllAnnotation(QList<stHighlightContent> &listres) {}
    virtual void clearSearch() {}
    virtual void docBasicInfo(stFileInfo &info);
    virtual void title(QString &title) {}
    virtual void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) {}
    virtual void getAnnotationText(const QString &struuid, QString &strtext, int ipage) {}
    virtual bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
    {
        return false;
    }
    virtual bool freshFile(QString file)
    {
        return false;
    }
    virtual void jumpToHighLight(const QString &uuid, int ipage) {}
    virtual void changeAnnotationColor(int ipage, const QString uuid, const QColor &color) {}
    void stopLoadPageThread();
    bool openFile(QString filepath);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
    QPoint global2RelativePoint(QPoint globalpoint);
    bool showMagnifier(QPoint point);
    int getCurrentPageNo();
    int currentLastPageNo();
    int fromFirstGetLastPageNo(int pagenum);
    int fromLastPageGetFirstPageNo();
    int currentPageNo();
    Page::Link *mouseBeOverLink(QPoint point);
    bool getSelectTextString(QString &st);
    bool loadPages();
    double adaptWidthAndShow(double width);
    double adaptHeightAndShow(double height);
    void findNext();
    void findPrev();
    bool loadData();
    bool loadDoc(QString);
    int getPageSNum();
    bool exitSlideModel();
    QVector<PageBase *> *getPages();
    PageBase *getPage(int index);
    void magnifierClear();
    void pageMove(double mvx, double mvy);
    //    bool isWordsBeLoad();
    bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3);
    bool showSlideModel();
    void cacularValueXY(int &xvalue, int &yvalue, int curpage, bool bsearch = true, QRectF rect = QRectF());
    int pointInWhichPage(QPoint &qpoint);
    bool setBookMarkState(int page, bool state);
    bool mouseSelectText(QPoint start, QPoint stop);
    void selectAllText();
    void setAutoPlaySlide(bool autoplay, int timemsec);
    bool getAutoPlaySlideStatu();
    void setViewFocus();
    QString addIconAnnotation(const QPoint &pos);
    void moveIconAnnotation(const QString &uuid, const QPoint &pos);
    bool iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    bool removeIconAnnotation(const QString &uuid, int ipage);
    double getMaxZoomratio();

signals:
    void signal_pageChange(int);
    void signal_searchRes(stSearchRes);
    void signal_searchover();
    void signal_loadDocumment(QString);
    void signal_bookMarkStateChange(int page, bool state);
    void signal_openResult(bool);
    void signale_autoplaytoend();

protected slots:
    void slot_vScrollBarValueChanged(int value);
    void slot_hScrollBarValueChanged(int value);
    void slot_MagnifierPixmapCacheLoaded(int pageno);
    void slot_searchValueAdd(stSearchRes res);
    void slot_searchover();
    void slot_docummentLoaded(bool result);
    void slot_dataLoaded(bool result);
    bool pageJump(int pagenum);
    void setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode);
    void scaleAndShow(double scale, RotateType_EM rotate);
    bool setViewModeAndShow(ViewMode_EM viewmode);
    void showSlideModelTimerOut();
//    void loadPageTimerOut();
    void autoplayslidTimerOut();
protected:
    void showSinglePage();
    void showFacingPage();
    void initConnect();
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
//    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) override;

    DocummentBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentBase)
};


#endif // DOCUMMENTBASE_H
