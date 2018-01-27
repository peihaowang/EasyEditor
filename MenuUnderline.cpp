
#include "common_headers.h"
#include "ComboBoxColor.h"
#include "MenuUnderline.h"

QMenuUnderline::QMenuUnderline(QWidget* parent)
	: QMenu(parent)
{
	m_pCmbColor = new QComboBoxColor(this);
	m_pCmbColor->setCurrentColor(QColor(Qt::black));
	QObject::connect(m_pCmbColor, SIGNAL(colorChose(QColor)), this, SLOT(onColorChose(QColor)));

	m_pCmbLineStyle = new QComboBox(this);
	{
		_CPairVector<QString, QTextCharFormat::UnderlineStyle> vStyles;
		vStyles["Single"] = QTextCharFormat::SingleUnderline;
		vStyles["Dash"] = QTextCharFormat::DashUnderline;
		vStyles["Dot"] = QTextCharFormat::DotLine;
		vStyles["Dash Dot"] = QTextCharFormat::DashDotLine;
		vStyles["Dash Dot Dot"] = QTextCharFormat::DashDotDotLine;
		vStyles["Wave"] = QTextCharFormat::WaveUnderline;
		vStyles["Spell Check"] = QTextCharFormat::SpellCheckUnderline;

		_CPairVector<QString, QTextCharFormat::UnderlineStyle>::const_iterator it;
		__EACH(it, vStyles){
			m_pCmbLineStyle->addItem(it->first, QVariant(it->second));
		}
		QObject::connect(m_pCmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(onStyleChose(int)));
	}

	QFormLayout* pLayoutMain = new QFormLayout;
	pLayoutMain->addRow("Color", m_pCmbColor);
	pLayoutMain->addRow("Style", m_pCmbLineStyle);

	QMenu::setLayout(pLayoutMain);
}

QColor QMenuUnderline::underlineColor() const
{
	return m_pCmbColor->currentColor();
}

QTextCharFormat::UnderlineStyle QMenuUnderline::underlineStyle() const
{
	return (QTextCharFormat::UnderlineStyle)m_pCmbLineStyle->itemData(m_pCmbLineStyle->currentIndex(), Qt::UserRole).toInt();
}

void QMenuUnderline::onColorChose(const QColor& clColor)
{
	emit colorChose(clColor);
}

void QMenuUnderline::onStyleChose(int nIndex)
{
	emit styleChose((QTextCharFormat::UnderlineStyle)m_pCmbLineStyle->itemData(nIndex, Qt::UserRole).toInt());
}
