#ifndef COMBOBOXCOLOR_H
#define COMBOBOXCOLOR_H

#include <QComboBox>
#include "cpp_utilities.h"

class QComboBoxColor : public QComboBox
{

	Q_OBJECT

protected:

	bool		m_bCanPickMoreColor;
	int		m_iLastItem;

public:

	QComboBoxColor(QWidget* parent = 0);

	QColor currentColor() const;
	void setCurrentColor(const QColor& clColor);

protected:

	void addColor(int nPos, const QString& sName, const QColor& clColor);

signals:

	void colorChose(const QColor& clColor);

protected slots:

	void onCurrentIndexChanged(int index);

};

#endif // COMBOBOXCOLOR_H
