
#include "common_headers.h"
#include "ComboBoxColor.h"

QComboBoxColor::QComboBoxColor(QWidget* parent)
	: QComboBox(parent)
	, m_bCanPickMoreColor(true)
	, m_iLastItem(-1)
{
	QComboBox::setIconSize(QSize(16, 14));
	_CPairVector<QString, QColor>::const_iterator it;
	addColor(-1, "Transparent", Qt::transparent);
	__EACH(it, g_xOpt.m_vColorsList){
		QString sName = it->first;
		QColor clColor = it->second;
		addColor(-1, sName, clColor);
	}
	QComboBox::addItem("More colors ...");
	QComboBox::setCurrentIndex(0);
	QComboBox::setMaxVisibleItems(32);

	QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

QColor QComboBoxColor::currentColor() const
{
	return QComboBox::itemData(QComboBox::currentIndex()).value<QColor>();
}

void QComboBoxColor::setCurrentColor(const QColor& clColor)
{
	bool bFound = false;
	for(int i = 0; i < QComboBox::count(); i++){
		QColor clItem = QComboBox::itemData(i).value<QColor>();
		if(clItem.isValid() && clItem == clColor){
			QComboBox::setCurrentIndex(i);
			bFound = true;
			break;
		}
	}

	if(!bFound){
		addColor(QComboBox::count() - 1, clColor.name(), clColor);
		QComboBox::setCurrentIndex(QComboBox::count() - 2);
	}
}

void QComboBoxColor::addColor(int nPos, const QString& sName, const QColor& clColor)
{
	QVariant v; v.setValue(clColor);
	QIcon xIcon;
	{
		QSize szIcon = QSize(16, 14);
		QPixmap xPixmap;
		if(clColor.alpha() != 0){
			xPixmap = _CStandardPixmapMaker::makeColorBlock(clColor, szIcon, Qt::SolidPattern, false, Qt::black, Qt::SolidLine, true, 0);
		}else{
			//2018.2.4 Painting adapted to Mac retina monitor
			QSize szImg = szIcon; if(QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps)) szImg *= 2;
			xPixmap = QPixmap(szImg); xPixmap.fill(Qt::transparent);
			if(QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps)) xPixmap.setDevicePixelRatio(2.0);
			QPainter xPainter(&xPixmap);
			xPainter.setRenderHint(QPainter::Antialiasing, true);

			xPainter.setPen(QPen(Qt::black));
			xPainter.save();
			xPainter.setBrush(QBrush(Qt::white));
			xPainter.drawRect(QRect(0, 0, szIcon.width(), szIcon.height()));
			xPainter.restore();

			xPainter.drawLine(0, 0, szIcon.width(), szIcon.height());

//			QPainter xPainter(&xPixmap);
//			xPainter.setPen(QPen(Qt::black));
//			xPainter.save();
//			xPainter.setBrush(QBrush(Qt::white));
//			xPainter.drawRect(QRect(0, 0, xPixmap.width() - 1, xPixmap.height() - 1));
//			xPainter.restore();

//			xPainter.setRenderHint(QPainter::Antialiasing, true);
//			xPainter.drawLine(0, 0, xPixmap.width() - 1, xPixmap.height() - 1);
		}
		xIcon = QIcon(xPixmap);
	}
	if(nPos < 0){
		QComboBox::addItem(xIcon, sName, v);
	}else{
		QComboBox::insertItem(nPos, xIcon, sName, v);
	}
}

void QComboBoxColor::onCurrentIndexChanged(int index)
{
	if(index == QComboBox::count() - 1){
		if(m_bCanPickMoreColor){
			QWidget* pParent=qobject_cast<QDialog*>(parent());
			if(!pParent) pParent=QApplication::activeWindow();
			QColorDialog dlgColor(currentColor(), pParent);
			dlgColor.setWindowTitle("Pick a color ...");
			if(dlgColor.exec() == QColorDialog::Accepted){
				QColor clColor = dlgColor.selectedColor();
				_CTempValueChange<bool> _tvc(m_bCanPickMoreColor, false); Q_UNUSED(_tvc);
				addColor(QComboBox::count() - 1, clColor.name(), clColor);
				QComboBox::setCurrentIndex(QComboBox::count() - 2);
			}else{
				QComboBox::setCurrentIndex(m_iLastItem);
			}
		}
	}else{
		m_iLastItem = QComboBox::currentIndex();
		emit colorChose(QComboBox::itemData(index).value<QColor>());
	}
}

