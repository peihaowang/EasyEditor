
#include "common_headers.h"

#include "MacUnifiedToolBar.h"

#if _UNIFIED_TOOLBAR_MAC_10_10

///////////////////////////////////////////////////////////

_CToolItemsGroup::_CToolItemsGroup(const QString& sGroupName, QWidget* parent)
	: QWidget(parent)
	, m_sGroupName(sGroupName)
{
	m_sQssForToolButton = "QToolButton{border: 1px solid transparent; border-radius: 5px; padding: 1px 6px;}"
			      "QToolButton::active{border-color: white; background: white;}"
			      "QToolButton::!active{background: #F4F4F4; border-color: #CACACA;}"
			      "QToolButton::active:pressed{background: #CACACA; border-color: #CACACA;}"
			      "QToolButton::active:checked{background: #9A9A9A; border-color: #9A9A9A;}"
			      "QToolButton::!active:checked{background: #DCDCDC; border-color: #CACACA;}"
			      "QToolButton[popupMode='1']{padding-right: 16px;}"
			      //"QToolButton::menu-button{border: 1px solid transparent; border-left: 0px none transparent;  padding-top: 3px; padding-bottom: 3px;}"
			      "QToolButton::menu-button{border-width: 1px; padding-top: 3px; padding-bottom: 3px; padding-right: 6px;}"
			      "QToolButton::menu-arrow{left: 0px; image: url(:/images/ico_dropdown.svg);}"
			      "QToolButton[checked='true'][__active='true']::menu-arrow{left: 0px; image: url(:/images/ico_dropdown_white.png);}"
			      ;

//	m_sQssForToolButton=g_xOpt.applyCustomImgDirInQss(m_sQssForToolButton);

	m_pLayoutToolItems = new QHBoxLayout;
	m_pLayoutToolItems->setSpacing(2);
	m_pLayoutToolItems->setContentsMargins(0, 0, 0, 0);

	QLabel* pStaticGroupName = new QLabel(sGroupName);
	pStaticGroupName->setAlignment(Qt::AlignCenter);
	pStaticGroupName->setVisible(false);

	QVBoxLayout* pMainLayout = new QVBoxLayout;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	pMainLayout->setContentsMargins(0, 0, 0, 0);
#else
	pMainLayout->setContentsMargins(5, 0, 5, 5);
#endif
	pMainLayout->setSpacing(2);
	pMainLayout->addLayout(m_pLayoutToolItems);
	pMainLayout->addWidget(pStaticGroupName);

	QWidget::setLayout(pMainLayout);
}

void _CToolItemsGroup::addAction(QAction* pAction)
{
	QToolButton* pToolBtn = new QToolButton(this);
	pToolBtn->setDefaultAction(pAction);

	addWidget(pToolBtn);
}

void _CToolItemsGroup::addWidget(QWidget* pWidget)
{
	m_pLayoutToolItems->addWidget(pWidget);

	QToolButton* pToolButton = qobject_cast<QToolButton*>(pWidget);
	if(pToolButton){
		pToolButton->setProperty("__active", QVariant(true));
		pToolButton->setStyleSheet(m_sQssForToolButton);

		{
			QGraphicsDropShadowEffect* pShadowEffect = new QGraphicsDropShadowEffect;
			pShadowEffect->setColor(QColor(100, 100, 100, 180));
			pShadowEffect->setOffset(0.5, 0.5);
			pShadowEffect->setBlurRadius(3.0);
			pToolButton->setGraphicsEffect(pShadowEffect);
			pToolButton->installEventFilter(this);
		}

		QObject::connect(pToolButton, SIGNAL(toggled(bool)), this, SLOT(onToggleToolButtonChecked(bool)));
	}
	m_vToolItems << pWidget;
}

bool _CToolItemsGroup::eventFilter(QObject* o, QEvent* e)
{
	QToolButton* pTB = qobject_cast<QToolButton*>(o);
	if(pTB){
		if(e->type() == QEvent::WindowDeactivate){
			QGraphicsEffect* pEffect = pTB->graphicsEffect();
			if(pEffect) pEffect->setEnabled(false);
			pTB->setProperty("__active", QVariant(false));
			pTB->setStyleSheet(m_sQssForToolButton);
		}else if(e->type() == QEvent::WindowActivate){
			QGraphicsEffect* pEffect = pTB->graphicsEffect();
			if(pEffect) pEffect->setEnabled(true);
			pTB->setProperty("__active", QVariant(true));
			pTB->setStyleSheet(m_sQssForToolButton);
		}else if(e->type() == QEvent::EnabledChange){
			QGraphicsEffect* pEffect = pTB->graphicsEffect();
			if(pEffect) pEffect->setEnabled(pTB->isEnabled());
		}
	}
	return QWidget::eventFilter(o, e);
}

void _CToolItemsGroup::onToggleToolButtonChecked(bool bChk)
{
	Q_UNUSED(bChk);
	QToolButton* pToolButton = qobject_cast<QToolButton*>(QObject::sender());
	if(pToolButton){
		pToolButton->setStyleSheet(m_sQssForToolButton);
	}
}

///////////////////////////////////////////////////////////

QUnifiedToolBar::QUnifiedToolBar(QWidget* parent)
	: QToolBar(parent)
{
	return;
}

QUnifiedToolBar::~QUnifiedToolBar()
{
}

QStringList QUnifiedToolBar::groups()
{
	QStringList vRes;
	_CPairVector<QString, QAction*>::const_iterator it;
	__EACH(it, m_mToolItemsGroups){
		vRes << it->first;
	}
	return vRes;
}

void QUnifiedToolBar::addSpacer(const QString& sGroupName)
{
	QWidget* pSpacer = new QWidget(this);
	pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	pSpacer->setMinimumWidth(0);
	pSpacer->setVisible(!m_mToolItemsGroups.empty());
	QAction* pSpacerAction = QToolBar::addWidget(pSpacer);
	m_mSpacerBeforeGroup[sGroupName] = pSpacerAction;
}

_CToolItemsGroup* QUnifiedToolBar::toolItemGroupOf(const QString& sGroupName)
{
	_CToolItemsGroup* pToolItemsGroup = NULL;
	if(!sGroupName.isEmpty()) {
		_CPairVector<QString, QAction*>::const_iterator it = m_mToolItemsGroups._find(sGroupName);
		if(it != m_mToolItemsGroups.end()){
			QWidgetAction* pWidgetAction = static_cast<QWidgetAction*>(it->second);
			if(pWidgetAction) pToolItemsGroup = qobject_cast<_CToolItemsGroup*>(pWidgetAction->defaultWidget());
		}else{
			addSpacer(sGroupName);

			pToolItemsGroup = new _CToolItemsGroup(sGroupName, this);
			QAction* pWidgetAction = QToolBar::addWidget(pToolItemsGroup);
			m_mToolItemsGroups[sGroupName] = pWidgetAction;
		}
	}
	return pToolItemsGroup;
}

void QUnifiedToolBar::addWidget(const QString& sGroupName, QWidget* pWidget)
{
	if(pWidget && !sGroupName.isEmpty()) {
		_CToolItemsGroup* pToolItemsGroup = toolItemGroupOf(sGroupName);
		if(pToolItemsGroup){
			pToolItemsGroup->addWidget(pWidget);
		}
	}
}

void QUnifiedToolBar::addAction(const QString& sGroupName, QAction* pAction)
{
	if(pAction && !sGroupName.isEmpty()){
		_CToolItemsGroup* pToolItemsGroup = toolItemGroupOf(sGroupName);
		if(pToolItemsGroup){
			pToolItemsGroup->addAction(pAction);
		}
	}
}

void QUnifiedToolBar::addSeparator(const QString& sGroupName)
{
	Q_UNUSED(sGroupName);
//	if(!sGroupName.isEmpty()) {
//		QAction* pAction = new QAction(this);
//		pAction->setSeparator(true);
//		if(pAction){
//			_CActionList* pActionGroup = NULL;
//			_CPairVector<QString, _CActionList*>::const_iterator it = m_mToolItemsGroups._find(sGroupName);
//			if(it != m_mToolItemsGroups.end()){
//				pActionGroup = it->second;
//			}else{
//				pActionGroup = new _CActionList;
//				m_mToolItemsGroups[sGroupName] = pActionGroup;
//			}
//			if(pActionGroup){
//				pActionGroup->push_back(pAction);
//				updateToolBar();
//			}
//		}
//	}
}

void QUnifiedToolBar::dockFirstToolbar()
{
	bool bFirstDocked = false;
	_CPairVector<QString, QAction*>::const_iterator it;
	__EACH(it, m_mToolItemsGroups){
		QString sGroupName = it->first;
		QAction* pWidgetAction = it->second;
		if(pWidgetAction){
			if(pWidgetAction->isVisible()){
				_CPairVector<QString, QAction*>::const_iterator iter = m_mSpacerBeforeGroup._find(sGroupName);
				if(iter != m_mSpacerBeforeGroup.end()){
					QAction* pSpacerAction = iter->second;
					if(pSpacerAction){
						pSpacerAction->setVisible(bFirstDocked);
						if(!bFirstDocked) bFirstDocked = true;
					}
				}
			}
		}
	}
}

bool QUnifiedToolBar::isGroupVisible(const QString& sGroupName)
{
	bool bVisible = false;
	_CPairVector<QString, QAction*>::const_iterator it = m_mToolItemsGroups._find(sGroupName);
	if(it != m_mToolItemsGroups.end()){
		QAction* pWidgetAction = it->second;
		if(pWidgetAction){
			bVisible = pWidgetAction->isVisible();
		}
	}

	return bVisible;
}

void QUnifiedToolBar::setGroupVisible(const QString& sGroupName, bool bVisible)
{
	{
		_CPairVector<QString, QAction*>::const_iterator it = m_mToolItemsGroups._find(sGroupName);
		if(it != m_mToolItemsGroups.end()){
			QAction* pWidgetAction = it->second;
			if(pWidgetAction){
				pWidgetAction->setVisible(bVisible);
			}
		}
	}

	{
		_CPairVector<QString, QAction*>::const_iterator it = m_mSpacerBeforeGroup._find(sGroupName);
		if(it != m_mSpacerBeforeGroup.end()){
			QAction* pSpacerAction = it->second;
			if(pSpacerAction){
				pSpacerAction->setVisible(bVisible);
			}
		}
	}

	dockFirstToolbar();
}

#else

///////////////////////////////////////////////////////////

QUnifiedToolBar::QUnifiedToolBar(QWidget* parent)
	: QToolBar(parent)
{
	return;
}

QUnifiedToolBar::~QUnifiedToolBar()
{
	_CPairVector<QString, _CActionList*>::const_iterator it;
	__EACH(it, m_mActionGroups){
		_CActionList* pActionList = it->second;
		if(pActionList){
			delete pActionList;
			pActionList = NULL;
		}
	}
}

QStringList QUnifiedToolBar::groups()
{
	QStringList vRes;
	_CPairVector<QString, _CActionList*>::const_iterator it;
	__EACH(it, m_mActionGroups){
		vRes << it->first;
	}
	return vRes;
}

void QUnifiedToolBar::addWidget(const QString& sGroupName, QWidget* pWidget)
{
	if(pWidget && !sGroupName.isEmpty()) {
		QWidgetAction* pAction = new QWidgetAction(this);
		pAction->setDefaultWidget(pWidget);// QToolBar::addWidget(pWidget);
		if(pAction){
			_CActionList* pActionGroup = NULL;
			_CPairVector<QString, _CActionList*>::const_iterator it = m_mActionGroups._find(sGroupName);
			if(it != m_mActionGroups.end()){
				pActionGroup = it->second;
			}else{
				bool bSpacer = !m_mActionGroups.empty();

				pActionGroup = new _CActionList;
				m_mActionGroups[sGroupName] = pActionGroup;

				if(bSpacer){
					QWidget* pSpacer = new QWidget();
					pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
					pSpacer->setMinimumWidth(0);
					addWidget(sGroupName, pSpacer);
				}

			}
			if(pActionGroup){
				pActionGroup->push_back(pAction);
				updateToolBar();
			}
		}
	}
}

void QUnifiedToolBar::addAction(const QString& sGroupName, QAction* pAction)
{
	if(pAction && !sGroupName.isEmpty()){
		_CActionList* pActionGroup = NULL;
		_CPairVector<QString, _CActionList*>::const_iterator it = m_mActionGroups._find(sGroupName);
		if(it != m_mActionGroups.end()){
			pActionGroup = it->second;
		}else{
			bool bSpacer = !m_mActionGroups.empty();

			pActionGroup = new _CActionList;
			m_mActionGroups[sGroupName] = pActionGroup;

			if(bSpacer){
				QWidget* pSpacer = new QWidget();
				pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
				pSpacer->setMinimumWidth(0);
				addWidget(sGroupName, pSpacer);
			}

		}
		if(pActionGroup){
			pActionGroup->push_back(pAction);
			updateToolBar();
		}
	}
}

void QUnifiedToolBar::addSeparator(const QString& sGroupName)
{
	if(!sGroupName.isEmpty()) {
		QAction* pAction = new QAction(this);
		pAction->setSeparator(true);
		if(pAction){
			_CActionList* pActionGroup = NULL;
			_CPairVector<QString, _CActionList*>::const_iterator it = m_mActionGroups._find(sGroupName);
			if(it != m_mActionGroups.end()){
				pActionGroup = it->second;
			}else{
				pActionGroup = new _CActionList;
				m_mActionGroups[sGroupName] = pActionGroup;
			}
			if(pActionGroup){
				pActionGroup->push_back(pAction);
				updateToolBar();
			}
		}
	}
}

bool QUnifiedToolBar::isGroupVisible(const QString& sGroupName)
{
	bool bVisible = false;
	_CPairVector<QString, _CActionList*>::const_iterator it = m_mActionGroups._find(sGroupName);
	if(it != m_mActionGroups.end()){
		_CActionList* pActionGroup = it->second;
		if(pActionGroup){
			bVisible = pActionGroup->isVisible();
		}
	}
	return bVisible;
}

void QUnifiedToolBar::setGroupVisible(const QString& sGroupName, bool bVisible)
{
	_CPairVector<QString, _CActionList*>::const_iterator it = m_mActionGroups._find(sGroupName);
	if(it != m_mActionGroups.end()){
		_CActionList* pActionGroup = it->second;
		if(pActionGroup){
			pActionGroup->setVisible(bVisible);
			updateToolBar();
		}
	}
}

//void QUnifiedToolBar::setActionAlignment(QAction* pAction, Qt::Alignment nAlign)
//{
//	QWidget* pWidget = QToolBar::widgetForAction(pAction);
//	setWidgetAlignment(pWidget, nAlign);
//}

//void QUnifiedToolBar::setWidgetAlignment(QWidget* pWidget, Qt::Alignment nAlign)
//{
//	if(pWidget){
//		QList<QAction*> vActions = QToolBar::actions();
//		QLayout* pLayout = QToolBar::layout();
//		for(int i = 0; i < vActions.size(); i++) {
//			QWidget* pActionWidget = QToolBar::widgetForAction(vActions[i]);
//			if(pActionWidget && pActionWidget == pWidget) {
//				pLayout->setAlignment(pWidget, nAlign);
//			}
//		}
//	}
//}

void QUnifiedToolBar::updateToolBar()
{
	QToolBar::clear();

	_CPairVector<QString, _CActionList*>::const_iterator it;
	__EACH(it, m_mActionGroups){
		_CActionList* pActionGroup = it->second;
		if(pActionGroup && pActionGroup->isVisible()){
			_CActionList::const_iterator iter;
			__EACH(iter, (*pActionGroup)){
				QAction* pAction = *iter;
				if(pAction){
					QToolBar::addAction(pAction);
				}
			}
		}
	}
}

#endif
