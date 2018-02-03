#ifndef COLORMENU_H
#define COLORMENU_H

#include <QMenu>
#include <QToolButton>

#include "app_settings.h"

class QGridLayout;
class QColorPicker;

class QActionColor : public QAction
{
	Q_OBJECT

public:

	enum _TAction{
		ApplyCurrentColor = 0
		, PickColorFromDlg
	};

protected:

	_TAction		m_iAction;

	QIcon			m_xIcon;
	QSize			m_szIcon;
	QColor			m_clSelected;

	QString			m_sDlgTitle;

protected:

	void setIconForColorBtn(const QColor& clColor);

public:

	QActionColor(const QString& sText, const QColor& clInit = QColor(Qt::transparent), QObject* parent = NULL);
	QActionColor(const QString& sText, const QIcon& xIcon, const QColor& clInit = QColor(Qt::transparent), QObject* parent = NULL);
	QActionColor(const QIcon& xIcon, const QColor& clInit = QColor(Qt::transparent), QObject* parent = NULL);

	_TAction triggerAction() const {return m_iAction;}
	void setTriggerAction(_TAction iAction){m_iAction = iAction;}

	QString colorDialogTitle() const {return m_sDlgTitle;}
	void setColorDialogTitle(const QString& sTitle){m_sDlgTitle = sTitle;}

	QColor currentColor() const {return m_clSelected;}

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onTriggered();

public slots:

	void setCurrentColor(const QColor& clCurrent){m_clSelected = clCurrent; setIconForColorBtn(clCurrent);}

};

///////////////////////////////////////////////////////////

class QMenuColor : public QMenu
{

	Q_OBJECT

protected:

	QColorPicker *	m_pColorPicker;

public:

	QMenuColor(QWidget* parent = 0);

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onColorChose(const QColor& clColor){QWidget::close(); emit colorChose(clColor);}

};

///////////////////////////////////////////////////////////

class QToolButtonColor : public QToolButton
{
	Q_OBJECT

protected:

	QActionColor *		m_pDefAction;
	QMenuColor *		m_pMenuColor;

public:

	QToolButtonColor(const QIcon& xIcon, const QColor& clInit, QWidget* parent);

	QColor currentColor() const {return m_pDefAction->currentColor();}

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onChooseColor(const QColor& clColor);

public slots:

	void setCurrentColor(const QColor& clCurrent){m_pDefAction->setCurrentColor(clCurrent);}

};

#endif // COLORMENU_H
