#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DListWidget>
#include <DImageButton>

#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QStringList>

#include "BookMarkItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */


class BookMarkWidget : public CustomWidget
{
    Q_OBJECT

public:
    BookMarkWidget(CustomWidget *parent = nullptr);

private slots:
    void slotShowSelectItem(QListWidgetItem *);
    void slotAddBookMark();

protected:
    void initWidget() override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void dltItem();
    void addBookMarkItem(const QImage &, const int &, const int &);
    void fillContantToList();

    inline int bookMarks() const
    {
        return m_bookMarks;
    }

    inline void setBookMarks(const int &marks)
    {
        m_bookMarks = marks;
    }

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private:
    DListWidget *m_pBookMarkListWidget = nullptr;
    QVBoxLayout *m_pVBoxLayout = nullptr;
    QListWidgetItem *m_pCurrentItem = nullptr;
    DImageButton *m_pAddBookMarkBtn = nullptr;

    int m_bookMarks = 0;       // 当前书签页总数
    QMap<int, bool> m_booksMap;// 当前书签页列表简略信息(页码(从1开始), )
};

#endif // BOOKMARKFORM_H
