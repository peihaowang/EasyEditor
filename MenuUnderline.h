#ifndef MENUUNDERLINE_H
#define MENUUNDERLINE_H

#include <QMenu>
#include <QToolButton>
#include <QTextCharFormat>

class QGridLayout;
class QComboBoxColor;
class QComboBox;

class QMenuUnderline : public QMenu
{

	Q_OBJECT

protected:

	QComboBoxColor *		m_pCmbColor;
	QComboBox *			m_pCmbLineStyle;

public:

	QMenuUnderline(QWidget* parent = 0);

	QColor underlineColor() const;
	QTextCharFormat::UnderlineStyle underlineStyle() const;

signals:

	void colorChose(const QColor& clColor);
	void styleChose(QTextCharFormat::UnderlineStyle iStyle);

protected slots:

	void onColorChose(const QColor& clColor);
	void onStyleChose(int nIndex);


};

#endif // COLORMENU_H
