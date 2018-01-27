#ifndef QDLGPARAGRAPH_H
#define QDLGPARAGRAPH_H

#include <QDialog>

class QLineEdit;

class QDlgParaMargin : public QDialog
{

protected:

	QLineEdit *		m_pEditStartIndent;
	QLineEdit *		m_pEditMarginLeft;
	QLineEdit *		m_pEditMarginRight;
	QLineEdit *		m_pEditMarginTop;
	QLineEdit *		m_pEditMarginBottom;

public:

	int			m_nStartIndent;
	int			m_nMarginLeft;
	int			m_nMarginRight;
	int			m_nMarginTop;
	int			m_nMarginBottom;

public:

	QDlgParaMargin(int nStartIndent, int nMarginLeft, int nMarginRight, int nMarginTop, int nMarginBottom, QWidget* parent);

public slots:

	virtual void accept();

};

#endif // QDLGPARAGRAPH_H
