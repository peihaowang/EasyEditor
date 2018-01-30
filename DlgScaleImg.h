#ifndef QDLGSCALEIMG_H
#define QDLGSCALEIMG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class QDlgScaleImg : public QDialog
{

	Q_OBJECT

protected:

	int			m_nOriginWidth;
	int			m_nOriginHeight;

	QLineEdit *		m_pEditWidth;
	QLineEdit *		m_pEditHeight;

	QCheckBox *		m_pChkKeepRatio;

	bool			m_bBlockChangeSignals;

public:

	int			m_nWidth;
	int			m_nHeight;

public:

	QDlgScaleImg(int nWidth, int nHeight, QWidget* parent);

protected slots:

	void onWidthChanged(const QString& sText);
	void onHeightChanged(const QString& sText);

public slots:

	virtual void accept();

};

#endif // QDLGSCALEIMG_H
