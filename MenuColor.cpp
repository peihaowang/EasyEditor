
#include "common_headers.h"
#include "ColorPicker.h"
#include "MenuColor.h"

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

QToolButtonColor::QToolButtonColor(const QPixmap& xIcon, const QColor& clInit, QWidget* parent)
	: QToolButton(parent)
	, m_xIcon(xIcon)
	, m_clSelected(clInit)
	, m_iPosition(Bottom)
{
	m_pDefAction = new QAction(this);
	QObject::connect(m_pDefAction, SIGNAL(triggered(bool)), this, SLOT(onDefActionTriggered()));

	QToolButton::setPopupMode(QToolButton::MenuButtonPopup);
	{
		m_pMenuColor = new QMenuColor(this);
		QToolButton::setMenu(m_pMenuColor);
		QObject::connect(m_pMenuColor, SIGNAL(colorChose(QColor)), this, SLOT(onChooseColor(QColor)));
	}
	QToolButton::setDefaultAction(m_pDefAction);
	setIconForColorBtn(clInit);
}

void QToolButtonColor::setIconForColorBtn(const QColor& clColor)
{
	const int nColorBarHeight = 3;
	int nImgWidth = m_xIcon.width(), nImgHeight = m_xIcon.height();

	QPixmap xPixmap = m_xIcon;
	QColor clBorder = clColor; if(clBorder.alpha() == 0) clBorder = QColor(Qt::black);
	QRect rc(0, nImgHeight - nColorBarHeight, nImgWidth - 1, nColorBarHeight - 1);
	QPainter xPainter(&xPixmap);
	xPainter.setPen(QPen(clBorder, 1.0, Qt::SolidLine));
	xPainter.setBrush(QBrush(clColor));
	xPainter.drawRect(rc);

	QToolButton::setIcon(QIcon(xPixmap));
}

void QToolButtonColor::onChooseColor(const QColor& clColor)
{
	if(m_clSelected != clColor){
		m_clSelected = clColor;
		setIconForColorBtn(clColor);
	}
	emit colorChose(clColor);
}
