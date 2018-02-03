
#include "common_headers.h"

#include "PanelFindReplace.h"

QPanelFindReplace::QPanelFindReplace(QWidget* parent)
	: QWidget(parent)
{
	QWidget::setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_pStaticTitle = new QLabel(this);
	m_pStaticTitle->setText("Find & Replace");

	m_pEditFind = new QLineEdit(this);
	m_pEditFind->setPlaceholderText("Text to find ...");
	QObject::connect(m_pEditFind, SIGNAL(textChanged(QString)), this, SLOT(onFindStrChanged()));
	QObject::connect(m_pEditFind, SIGNAL(returnPressed()), this, SLOT(onFindNext()));

	m_pBtnFindPrevious = new QToolButton(this);
	m_pBtnFindPrevious->setAutoRaise(true);
	m_pBtnFindPrevious->setEnabled(false);
	m_pBtnFindPrevious->setShortcut(QKeySequence::FindPrevious);
	m_pBtnFindPrevious->setIcon(QIcon(":/images/btn_find_previous.png"));
	QObject::connect(m_pBtnFindPrevious, SIGNAL(clicked()), this, SLOT(onFindPrevious()));

	m_pBtnFindNext = new QToolButton(this);
	m_pBtnFindNext->setAutoRaise(true);
	m_pBtnFindNext->setEnabled(false);
	m_pBtnFindNext->setShortcut(QKeySequence::FindNext);
	m_pBtnFindNext->setIcon(QIcon(":/images/btn_find_next.png"));
	QObject::connect(m_pBtnFindNext, SIGNAL(clicked()), this, SLOT(onFindNext()));

	m_pEditReplace = new QLineEdit(this);
	m_pEditReplace->setPlaceholderText("Replace with ...");
	QObject::connect(m_pEditReplace, SIGNAL(returnPressed()), this, SLOT(onReplace()));

	m_pBtnReplace = new QToolButton(this);
	m_pBtnReplace->setAutoRaise(true);
	m_pBtnReplace->setEnabled(false);
	m_pBtnReplace->setText("Replace");
	m_pBtnReplace->setToolTip("Replace current match");
	QObject::connect(m_pBtnReplace, SIGNAL(clicked()), this, SLOT(onReplace()));

	m_pBtnReplaceAll = new QToolButton(this);
	m_pBtnReplaceAll->setAutoRaise(true);
	m_pBtnReplaceAll->setEnabled(false);
	m_pBtnReplaceAll->setText("Replace All");
	m_pBtnReplaceAll->setToolTip("Replace all matches");
	QObject::connect(m_pBtnReplaceAll, SIGNAL(clicked()), this, SLOT(onReplaceAll()));

	m_pChkCase = new QCheckBox(this);
	m_pChkCase->setText("Case Sensitive");
	m_pChkCase->setChecked(g_xOpt.m_bFindTextWithCase);
	QObject::connect(m_pChkCase, SIGNAL(toggled(bool)), this, SLOT(onToggleCase(bool)));

	m_pChkWholeWord = new QCheckBox(this);
	m_pChkWholeWord->setText("Match Whole Word");
	m_pChkWholeWord->setChecked(g_xOpt.m_bFindTextWholeWord);
	QObject::connect(m_pChkWholeWord, SIGNAL(toggled(bool)), this, SLOT(onToggleWholeWord(bool)));

	m_pChkUseRegExp = new QCheckBox(this);
	m_pChkUseRegExp->setText("Use Regular Expressions");
	m_pChkUseRegExp->setChecked(g_xOpt.m_bFindTextRegExp);
	QObject::connect(m_pChkUseRegExp, SIGNAL(toggled(bool)), this, SLOT(onToggleUseRegExp(bool)));

	m_pChkHighlight = new QCheckBox(this);
	m_pChkHighlight->setText("Highlight All Occurrences");
	m_pChkHighlight->setChecked(g_xOpt.m_bFindTextHighlightAll);
	QObject::connect(m_pChkHighlight, SIGNAL(toggled(bool)), this, SLOT(onToggleHighlightAll(bool)));

	m_pBtnClose = new QToolButton(this);
	m_pBtnClose->setAutoRaise(true);
	m_pBtnClose->setIcon(m_pBtnClose->style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
	m_pBtnClose->setIconSize(QSize(10, 10));
	m_pBtnClose->setToolTip("Close");
#if defined(Q_OS_MAC)
	m_pBtnClose->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
		"QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
		"QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
	);
#endif

	QObject::connect(m_pBtnClose, SIGNAL(clicked()), this, SLOT(onClosePanel()));

	QHBoxLayout* pLayoutTop = new QHBoxLayout;
	pLayoutTop->setContentsMargins(0, 0, 0, 0);
	pLayoutTop->addWidget(m_pStaticTitle);
	pLayoutTop->addStretch();
	pLayoutTop->addWidget(m_pBtnClose);

	QHBoxLayout* pLayoutCenter = new QHBoxLayout;
	pLayoutCenter->setSpacing(4);
	pLayoutCenter->addWidget(m_pEditFind);
	pLayoutCenter->addWidget(m_pBtnFindPrevious);
	pLayoutCenter->addWidget(m_pBtnFindNext);
	pLayoutCenter->addWidget(m_pEditReplace);
	pLayoutCenter->addWidget(m_pBtnReplace);
	pLayoutCenter->addWidget(m_pBtnReplaceAll);

	QHBoxLayout* pLayoutBottom = new QHBoxLayout;
	pLayoutBottom->setContentsMargins(0, 0, 0, 0);
	pLayoutBottom->addWidget(m_pChkCase);
	pLayoutBottom->addWidget(m_pChkWholeWord);
	pLayoutBottom->addWidget(m_pChkUseRegExp);
	pLayoutBottom->addWidget(m_pChkHighlight);
	pLayoutBottom->addStretch();

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->setContentsMargins(6, 3, 6, 3);
	pLayoutMain->addLayout(pLayoutTop);
	pLayoutMain->addLayout(pLayoutCenter);
	pLayoutMain->addLayout(pLayoutBottom);

	QWidget::setLayout(pLayoutMain);
}

void QPanelFindReplace::setFocusOnFindInput()
{
	m_pEditFind->setFocus();
	m_pEditFind->selectAll();
}

void QPanelFindReplace::setFocusOnReplaceInput()
{
	m_pEditReplace->setFocus();
	m_pEditReplace->selectAll();
}

void QPanelFindReplace::highlight(bool b)
{
	if(b){
		emit highlightOccurrences(m_pEditFind->text(), m_pChkCase->isChecked(), m_pChkWholeWord->isChecked(), m_pChkUseRegExp->isChecked());
	}else{
		emit highlightOccurrences("", false, false, false);
	}
}

void QPanelFindReplace::setVisible(bool bVisible)
{
	if(bVisible != QWidget::isVisible()){
		highlight(bVisible && m_pChkHighlight->isChecked());
		emit visibilityChanged(bVisible);
	}
	QWidget::setVisible(bVisible);
}

QString QPanelFindReplace::findString() const {return m_pEditFind->text();}

void QPanelFindReplace::setFindString(const QString& sText){m_pEditFind->setText(sText);}

QString QPanelFindReplace::replaceWithString() const {return m_pEditReplace->text();}

bool QPanelFindReplace::isCaseSensitive() const {return m_pChkCase->isChecked();}

bool QPanelFindReplace::isMatchWholeWord() const {return m_pChkWholeWord->isChecked();}

bool QPanelFindReplace::isHighlightAll() const {return m_pChkHighlight->isChecked();}

bool QPanelFindReplace::useRegExp() const {return m_pChkUseRegExp->isChecked();}

void QPanelFindReplace::onFindStrChanged()
{
	QString sTxtToFind = m_pEditFind->text();

	bool bEmpty=sTxtToFind.isEmpty();

	highlight(!bEmpty && m_pChkHighlight->isChecked());

	m_pBtnFindNext->setEnabled(!bEmpty);
	m_pBtnFindPrevious->setEnabled(!bEmpty);

	m_pBtnReplace->setEnabled(!bEmpty);
	m_pBtnReplaceAll->setEnabled(!bEmpty);
}

void QPanelFindReplace::onFindNext()
{
	highlight(m_pChkHighlight->isChecked());

	QString sTxtToFind = m_pEditFind->text();
	if(!sTxtToFind.isEmpty()){
		emit findNext(sTxtToFind, true, m_pChkCase->isChecked(), m_pChkWholeWord->isChecked(), false, m_pChkUseRegExp->isChecked());
	}
}

void QPanelFindReplace::onFindPrevious()
{
	highlight(m_pChkHighlight->isChecked());

	QString sTxtToFind = m_pEditFind->text();
	if(!sTxtToFind.isEmpty()){
		emit findNext(sTxtToFind, true, m_pChkCase->isChecked(), m_pChkWholeWord->isChecked(), m_pChkUseRegExp->isChecked(), true);
	}
}

void QPanelFindReplace::onReplace()
{
	QString sTxtToFind = m_pEditFind->text();
	if(!sTxtToFind.isEmpty()){
		emit replace(sTxtToFind, m_pEditReplace->text(), true, m_pChkCase->isChecked(), m_pChkWholeWord->isChecked(), m_pChkUseRegExp->isChecked());
	}
}

void QPanelFindReplace::onReplaceAll()
{
	QString sTxtToFind = m_pEditFind->text();
	if(!sTxtToFind.isEmpty()){
		emit replaceAll(sTxtToFind, m_pEditReplace->text(), m_pChkCase->isChecked(), m_pChkWholeWord->isChecked(), m_pChkUseRegExp->isChecked());
	}
}

void QPanelFindReplace::onToggleCase(bool b)
{
	g_xOpt.m_bFindTextWithCase = b;
	highlight(m_pChkHighlight->isChecked());
}

void QPanelFindReplace::onToggleWholeWord(bool b)
{
	g_xOpt.m_bFindTextWholeWord = b;
	highlight(m_pChkHighlight->isChecked());
}

void QPanelFindReplace::onToggleUseRegExp(bool b)
{
	g_xOpt.m_bFindTextRegExp = b;
	highlight(m_pChkHighlight->isChecked());
}

void QPanelFindReplace::onToggleHighlightAll(bool b)
{
	g_xOpt.m_bFindTextHighlightAll = b;
	highlight(b);
}
