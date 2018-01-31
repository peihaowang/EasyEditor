#ifndef UNIFIEDTOOLBAR_H
#define UNIFIEDTOOLBAR_H

#include <QToolBar>
#include "cpp_utilities.h"

#define		_UNIFIED_TOOLBAR_MAC_10_10		1 //2015.12.3 Mac-10.10 style toolbar?

///////////////////////////////////////////////////////////

class _CActionList : public std::vector<QAction*>{

protected:

	bool		m_bVisible;

public:

	_CActionList() : m_bVisible(true){return;}

	bool isVisible(){return m_bVisible;}
	void setVisible(bool bVisible){m_bVisible= bVisible;}
};

///////////////////////////////////////////////////////////

#if _UNIFIED_TOOLBAR_MAC_10_10

class QToolButton;
class QHBoxLayout;

///////////////////////////////////////////////////////////

class _CToolItemsGroup : public QWidget{

	Q_OBJECT

protected:

	QString			m_sGroupName;
	QHBoxLayout *		m_pLayoutToolItems;
	QList<QWidget*>		m_vToolItems;

	QString			m_sQssForToolButton;

public:

	_CToolItemsGroup(const QString& sGroupName = "", QWidget* parent = NULL);

	void addAction(QAction* pAction);
	void addWidget(QWidget* pWidget);
//	void addSeparator();

public:

	virtual bool eventFilter(QObject* o, QEvent* e);

protected slots:

	void onToggleToolButtonChecked(bool bChk);

};

////////////////////////////////////////////////////

class QUnifiedToolBar : public QToolBar{

protected:

	_CPairVector<QString, QAction*>		m_mToolItemsGroups;
	_CPairVector<QString, QAction*>		m_mSpacerBeforeGroup;

protected:

	void addSpacer(const QString& sGroupName);
	_CToolItemsGroup* toolItemGroupOf(const QString& sGroupName);

	void dockFirstToolbar();

public:

	QUnifiedToolBar(QWidget* parent = NULL);
	virtual ~QUnifiedToolBar();

	QStringList groups();

	void addWidget(const QString& sGroupName, QWidget* pWidget);
	void addAction(const QString& sGroupName, QAction* pAction);
	void addSeparator(const QString& sGroupName);

	bool isGroupVisible(const QString& sGroupName);
	void setGroupVisible(const QString& sGroupName, bool bVisible);

//	void setActionAlignment(QAction* pAction, Qt::Alignment nAlign);
//	void setWidgetAlignment(QWidget* pWidget, Qt::Alignment nAlign);

};

#else

class QUnifiedToolBar : public QToolBar{

protected:

	_CPairVector<QString, _CActionList*>		m_mActionGroups;

	//2015.1.25 Set to false to pause refreshing toolbar when add action/widget(for efficiency), and when set to true, it will cause refreshing
	bool						m_bPauseRefresh;

protected:

	void updateToolBar();

public:

	QUnifiedToolBar(QWidget* parent = NULL);
	virtual ~QUnifiedToolBar();

	void pauseRefreshToolbar(){m_bPauseRefresh = true;}
	void resumeRefreshToolbar(){m_bPauseRefresh = false;}

	QStringList groups();

	void addWidget(const QString& sGroupName, QWidget* pWidget);
	void addAction(const QString& sGroupName, QAction* pAction);
	void addSeparator(const QString& sGroupName);

	bool isGroupVisible(const QString& sGroupName);
	void setGroupVisible(const QString& sGroupName, bool bVisible);

//	void setActionAlignment(QAction* pAction, Qt::Alignment nAlign);
//	void setWidgetAlignment(QWidget* pWidget, Qt::Alignment nAlign);

};

#endif

#endif // UNIFIEDTOOLBAR_H
