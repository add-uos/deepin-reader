#include "NoteTipWidget.h"
#include <DPlatformWindowHandle>
#include <DTextEdit>
#include <QHBoxLayout>
#include <DFontSizeManager>
#include "CustomControl/CustomClickLabel.h"

NoteTipWidget::NoteTipWidget(DWidget *parnet)
    : DWidget(parnet)
    , m_iwidth(254)
{
    setWindowFlags(Qt::ToolTip);
    //  setFixedSize(200, 50);
    setFixedWidth(m_iwidth);
    DPlatformWindowHandle handle(this);
    int radius = 8;
    handle.setWindowRadius(radius);
    setWindowOpacity(0.97);

    initWidget();
    initConnection();
    slotUpdateTheme();
}

void NoteTipWidget::setTipContent(const QString &content)
{
    DTextEdit *pedit = this->findChild<DTextEdit *>();
    if (pedit) {
        QFontMetrics fm(pedit->font());
        int pixelsWide = fm.horizontalAdvance(content);
        int pixelsHigh = fm.height();
        //        qDebug() << pixelsWide << pixelsHigh << fm.lineSpacing();
        QString strcontent;
        //判断是否超过十行
        if (pixelsWide > (m_iwidth - 30) * 10) {
            setFixedHeight(pixelsHigh * 10 + 8);
            strcontent = pedit->fontMetrics().elidedText(content, Qt::ElideRight,
                                                         (m_iwidth - 30) * 10, Qt::TextWordWrap);
        } else {
            int line = pixelsWide / (m_iwidth - 16);
            // line = line > 2 ? line + 2 : 3;
            line = line < 1 ? 1 : line + 1;
            setFixedHeight(pixelsHigh * line + 8);
            strcontent = content;
        }
        pedit->setFixedWidth(m_iwidth - 16);
        pedit->setText(strcontent);
    }
}

void NoteTipWidget::initConnection()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ]() {
        slotUpdateTheme();
    });
}

void NoteTipWidget::slotUpdateTheme()
{
    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
    plt.setColor(Dtk::Gui::DPalette::WindowText, QPalette::Text);
    pedit->setPalette(plt);
}

void NoteTipWidget::initWidget()
{
    pedit = new QTextEdit(this);
    pedit->setFrameShape(QFrame::NoFrame);
    pedit->setReadOnly(true);
    pedit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pedit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pedit->setWordWrapMode(QTextOption::WrapAnywhere /*WordWrap*/);
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(pedit);
    DFontSizeManager::instance()->bind(pedit, DFontSizeManager::T8);
    this->setLayout(layout);
}
