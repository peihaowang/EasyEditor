
#include "common_headers.h"
#include "ColorPicker.h"
#include "MenuColor.h"

///////////////////////////////////////////////////////////

QActionColor::QActionColor(const QString& sText, const QColor& clInit, QObject* parent)
	: QAction(sText, parent)
	, m_iAction(ApplyCurrentColor)
	, m_clSelected(clInit)
{
	setIconForColorBtn(m_clSelected);
	QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered()));
}

QActionColor::QActionColor(const QString& sText, const QIcon& xIcon, const QColor &clInit, QObject* parent)
	: QAction(sText, parent)
	, m_iAction(ApplyCurrentColor)
	, m_xIcon(xIcon)
	, m_clSelected(clInit)
{
	setIconForColorBtn(m_clSelected);
	QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered()));
}

QActionColor::QActionColor(const QIcon& xIcon, const QColor& clInit, QObject* parent)
	: QAction(parent)
	, m_iAction(ApplyCurrentColor)
	, m_xIcon(xIcon)
	, m_clSelected(clInit)
{
	setIconForColorBtn(m_clSelected);
	QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered()));
}

void QActionColor::onTriggered()
{
	if(m_iAction == ApplyCurrentColor){
		emit colorChose(m_clSelected);
	}else if(m_iAction == PickColorFromDlg){
		QString sTitle = m_sDlgTitle; if(!sTitle.isEmpty()) sTitle = "Pick a color ...";
		QColorDialog dlgColor(m_clSelected, QApplication::activeWindow());
		dlgColor.setWindowTitle(sTitle);
		if(dlgColor.exec() == QColorDialog::Accepted){
			QColor clColor = dlgColor.selectedColor();
			m_clSelected = clColor;

			emit colorChose(clColor);
		}
	}
}

void QActionColor::setIconForColorBtn(const QColor& clColor)
{
	int nImgWidth = g_xOpt.m_szIconSize.width(), nImgHeight = g_xOpt.m_szIconSize.height();

	if(!m_xIcon.isNull()){
		QSize szPixmap(nImgWidth, nImgHeight); double nDevicePxRatio = 1.0;
		if(QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps)){
			szPixmap *= 2;
			nDevicePxRatio = 2.0;
		}
		QPixmap xPixmap(szPixmap); xPixmap.setDevicePixelRatio(nDevicePxRatio);
		xPixmap.fill(Qt::transparent);

		const int nRibbonHeight = 3;
		QColor clBorder = clColor; if(clBorder.alpha() == 0) clBorder = QColor(Qt::black);

		QPixmap xIconOrigin = m_xIcon.pixmap(g_xOpt.m_szIconSize);
//		xIconOrigin.setDevicePixelRatio(1.0); //2018.2.2 Keep origin pixmap for material usage (painting to another canvas)

		QRect rcIconTarget(0, 0, nImgWidth - 1, nImgHeight - nRibbonHeight);		//2018.2.2 Display level
		QRect rcIconSource(0, 0, nImgWidth * 2, (nImgHeight - nRibbonHeight) * 2);	//2018.2.2 Pixel level
		QRect rcRibbon(0, nImgHeight - nRibbonHeight, nImgWidth - 1, nRibbonHeight - 1);

		QPainter xPainter(&xPixmap);
		xPainter.drawPixmap(rcIconTarget, xIconOrigin, rcIconSource);
		xPainter.setPen(QPen(clBorder, 1.0, Qt::SolidLine));
		xPainter.setBrush(QBrush(clColor));
		xPainter.drawRect(rcRibbon);

		QAction::setIcon(QIcon(xPixmap));
	}else{
		QAction::setIcon(QIcon(_CStandardPixmapMaker::makeColorBlock(clColor, QSize(nImgWidth, nImgHeight))));
	}
}

///////////////////////////////////////////////////////////

QMenuColor::QMenuColor(QWidget* parent)
	: QMenu(parent)
{
	m_pColorPicker = new QColorPicker(this);
	QObject::connect(m_pColorPicker, SIGNAL(colorChose(QColor)), this, SLOT(onColorChose(QColor)));

	QVBoxLayout* pMainLayout = new QVBoxLayout;
	pMainLayout->setAlignment(Qt::AlignCenter);
	pMainLayout->setContentsMargins(5, 5, 5, 5);
	pMainLayout->setSpacing(2);
	pMainLayout->addWidget(m_pColorPicker);

	QWidget::setLayout(pMainLayout);
}

///////////////////////////////////////////////////////////

QToolButtonColor::QToolButtonColor(const QIcon& xIcon, const QColor& clInit, QWidget* parent)
	: QToolButton(parent)
{
	m_pDefAction = new QActionColor(xIcon, clInit, this);
	m_pDefAction->setTriggerAction(QActionColor::ApplyCurrentColor);
	QObject::connect(m_pDefAction, SIGNAL(colorChose(QColor)), this, SLOT(onChooseColor(QColor)));
	QToolButton::setDefaultAction(m_pDefAction);

	m_pMenuColor = new QMenuColor(this);
	QObject::connect(m_pMenuColor, SIGNAL(colorChose(QColor)), this, SLOT(onChooseColor(QColor)));
	QToolButton::setMenu(m_pMenuColor);
	QToolButton::setPopupMode(QToolButton::MenuButtonPopup);
}

void QToolButtonColor::onChooseColor(const QColor& clColor)
{
	m_pDefAction->setCurrentColor(clColor);
	emit colorChose(clColor);
}
