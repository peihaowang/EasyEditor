#ifndef COLORMENU_H
#define COLORMENU_H

#include <QMenu>
#include <QToolButton>

#include "app_settings.h"

class QGridLayout;
class QColorPicker;

class QMenuColor : public QMenu
{

	Q_OBJECT

protected:

	QColorPicker *	m_pColorPicker;

protected:

	void addColor(const QString& sName, const QColor& clColor);

public:

	QMenuColor(QWidget* parent = 0);

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onColorChose(const QColor& clColor){QWidget::close(); emit colorChose(clColor);}

};

class QToolButtonColor : public QToolButton
{
	Q_OBJECT

public:

	enum _TMarkPosition{
		Left = 1
		, Right
		, Top
		, Bottom
	};

protected:

	QPixmap			m_xIcon;
	QColor			m_clSelected;
	_TMarkPosition		m_iPosition;

	QAction *		m_pDefAction;
	QMenuColor *		m_pMenuColor;

protected:

	void setIconForColorBtn(const QColor& clColor);

public:

	QToolButtonColor(const QPixmap& xIcon, const QColor& clInit, QWidget* parent);

	_TMarkPosition markPosition() const {return m_iPosition;}
	void setMarkPosition(_TMarkPosition iPosition){m_iPosition = iPosition;}

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onDefActionTriggered(){onChooseColor(m_clSelected);}
	void onChooseColor(const QColor& clColor);

};

#endif // COLORMENU_H
