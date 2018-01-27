#ifndef QDLGSCALEIMG_H
#define QDLGSCALEIMG_H

#include <QDialog>

class QLineEdit;

class QDlgScaleImg : public QDialog
{

protected:

	QLineEdit *		m_pEditWidth;
	QLineEdit *		m_pEditHeight;

public:

	int			m_nWidth;
	int			m_nHeight;

public:

	QDlgScaleImg(int nWidth, int nHeight, QWidget* parent);

public slots:

	virtual void accept();

};

#endif // QDLGSCALEIMG_H
