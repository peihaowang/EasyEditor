#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class QToolButton;
class QGridLayout;
class QButtonGroup;

class QColorPicker : public QWidget
{

	Q_OBJECT

protected:

	QToolButton*	m_pBtnTransparent;
	QGridLayout*	m_pGridLayout;
	QButtonGroup*	m_pButtonsGroup;

	int		m_nCount;
	int		m_nNumOfCols;

	bool		m_bColorCheckable;

	QColor		m_clCurrent;
	QList<QColor>	m_vAddedColors;

protected:

	QToolButton* addColor(const QString& sName, const QColor& clColor);

public:

	QColorPicker(QWidget* parent = 0);

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onColorBtnClicked();
	void onTransparentColor();
	void onMoreColor();

};

#endif // QCOLORPICKER_H
