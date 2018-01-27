#ifndef FINDBOX_H
#define FINDBOX_H

#include <QWidget>

class QLineEdit;
class QQueryInput;
class QToolButton;
class QLabel;
class QCheckBox;

class QStyleCustomPanel;

class QPanelFindReplace : public QWidget {

	Q_OBJECT

protected:

	QLabel *		m_pStaticTitle;
	QLineEdit *		m_pEditFind;
	QLineEdit *		m_pEditReplace;

	QToolButton *		m_pBtnFindNext;
	QToolButton *		m_pBtnFindPrevious;
	QToolButton *		m_pBtnReplace;
	QToolButton *		m_pBtnReplaceAll;

	QToolButton *		m_pBtnClose;

	QCheckBox *		m_pChkCase;
	QCheckBox *		m_pChkWholeWord;
	QCheckBox *		m_pChkUseRegExp;
	QCheckBox *		m_pChkHighlight;

public:

	QPanelFindReplace(QWidget* parent);

	virtual void setVisible(bool bVisible);

	void setFocusOnFindInput();
	void setFocusOnReplaceInput();

	void highlight(bool b);

	QString findString() const;
	void setFindString(const QString& sText);

	QString replaceWithString() const;
	bool isCaseSensitive() const;
	bool isMatchWholeWord() const;
	bool isHighlightAll() const;
	bool useRegExp() const;

protected slots:

	void onFindStrChanged();

	void onFindNext();
	void onFindPrevious();

	void onReplace();
	void onReplaceAll();

	void onToggleCase(bool b);
	void onToggleWholeWord(bool b);
	void onToggleUseRegExp(bool b);
	void onToggleHighlightAll(bool b);

	void onClosePanel(){setVisible(false);}

signals:

	void highlightOccurrences(const QString& sText, bool bCaseSensitively, bool bWholeWord, bool bUseRegExp);
	void findNext(const QString& sExp, bool bWrapsAround, bool bCaseSensitively, bool bWholeWord, bool bUseRegExp, bool bBackWard);

	void replace(const QString& sExp, const QString& sReplaceWith, bool bWrapsAround, bool bCaseSensitively, bool bWholeWord, bool bUseRegExp);
	void replaceAll(const QString& sExp, const QString& sReplaceWith, bool bCaseSensitively, bool bWholeWord, bool bUseRegExp);

	void visibilityChanged(bool bVisible);

};

#endif
