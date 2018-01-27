
#include "common_headers.h"
#include "ColorPicker.h"

QColorPicker::QColorPicker(QWidget* parent)
	: QWidget(parent)
	, m_nCount(0)
	, m_nNumOfCols(8)
	, m_clCurrent(0, 0, 0, 0)
{
	m_pButtonsGroup = new QButtonGroup(this);
	m_pButtonsGroup->setExclusive(true);

	QAction* pTransparentColor = new QAction(this);
	pTransparentColor->setText("Transparent");
	pTransparentColor->setData(QVariant(QColor(Qt::transparent)));
	QObject::connect(pTransparentColor, SIGNAL(triggered()), this, SLOT(onTransparentColor()));
	m_pBtnTransparent = new QToolButton(this);
	m_pBtnTransparent->setDefaultAction(pTransparentColor);
	m_pBtnTransparent->setToolButtonStyle(Qt::ToolButtonTextOnly);
	m_pBtnTransparent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_pBtnTransparent->setAutoRaise(true);
#if defined(Q_OS_MAC)
    m_pBtnTransparent->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
        "QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
        "QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
    );
#endif
    m_pButtonsGroup->addButton(m_pBtnTransparent);

	m_pGridLayout = new QGridLayout;
	m_pGridLayout->setAlignment(Qt::AlignCenter);
	m_pGridLayout->setContentsMargins(0, 0, 0, 0);
	m_pGridLayout->setSpacing(0);

	_CPairVector<QString, QColor>::const_iterator it;
	__EACH(it, g_xOpt.m_vColorsList){
		QString sName = it->first;
		QColor clColor = it->second;
		addColor(sName, clColor);
	}

	QAction* pMoreColor = new QAction(this);
	pMoreColor->setText("More colors ...");
	QObject::connect(pMoreColor, SIGNAL(triggered()), this, SLOT(onMoreColor()));
	QToolButton* pMoreColorBtn = new QToolButton(this);
	pMoreColorBtn->setDefaultAction(pMoreColor);
	pMoreColorBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
	pMoreColorBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pMoreColorBtn->setAutoRaise(true);
#if defined(Q_OS_MAC)
    pMoreColorBtn->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
        "QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
        "QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
    );
#endif

	QVBoxLayout* pMainLayout = new QVBoxLayout;
	pMainLayout->setAlignment(Qt::AlignCenter);
	pMainLayout->setContentsMargins(5, 5, 5, 5);
	pMainLayout->setSpacing(2);

	pMainLayout->addWidget(m_pBtnTransparent);
	pMainLayout->addLayout(m_pGridLayout);
	pMainLayout->addWidget(pMoreColorBtn);

	QWidget::setLayout(pMainLayout);
}

QToolButton* QColorPicker::addColor(const QString& sName, const QColor& clColor)
{
	QAction* pAction = new QAction(this);
	pAction->setCheckable(m_bColorCheckable);
	pAction->setToolTip(sName);
	QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(onColorBtnClicked()));

	QSize szIcon = QSize(16, 14);
	{
		QPixmap xIcon = _CStandardPixmapMaker::makeColorBlock(clColor, szIcon, Qt::SolidPattern, false, Qt::black, Qt::SolidLine, true, 0);
		pAction->setIcon(QIcon(xIcon));
	}
	QVariant v; v.setValue(clColor);
	pAction->setData(v);

	QToolButton* pColorBtn = new QToolButton(this);
#if defined(Q_OS_MAC)
	pColorBtn->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
		"QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
		"QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
	);
#endif
	pColorBtn->setDefaultAction(pAction);
	pColorBtn->setIconSize(szIcon);
	pColorBtn->setAutoRaise(true);

	int nRow = ::floor(m_nCount / m_nNumOfCols);
	int nCol = m_nCount % m_nNumOfCols;
	m_pGridLayout->addWidget(pColorBtn, nRow, nCol);
	m_pButtonsGroup->addButton(pColorBtn);
	m_nCount++;

	return pColorBtn;
}

void QColorPicker::onColorBtnClicked()
{
	QAction* pAction = qobject_cast<QAction*>(QObject::sender());
	if(pAction){
		QColor clColor = pAction->data().value<QColor>();
		if(clColor.isValid()){
			m_clCurrent = clColor;
			emit colorChose(clColor);
		}
	}
}

void QColorPicker::onTransparentColor()
{
	m_clCurrent = QColor(Qt::transparent);
	emit colorChose(QColor(Qt::transparent));
}

void QColorPicker::onMoreColor()
{
	QColorDialog dlgColor(m_clCurrent, QApplication::activeWindow());
	dlgColor.setWindowTitle("Pick a color ...");
	if(dlgColor.exec() == QColorDialog::Accepted){
		QColor clColor = dlgColor.selectedColor();

		_CPairVector<QString, QColor>::const_iterator it = g_xOpt.m_vColorsList._find(clColor);
		if(it == g_xOpt.m_vColorsList.end()){
			QString sName = clColor.name();
			g_xOpt.m_vColorsList[sName] = clColor;
			addColor(sName, clColor);
		}
		m_clCurrent = clColor;

		emit colorChose(clColor);
	}
}
