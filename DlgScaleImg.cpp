
#include "common_headers.h"
#include "DlgScaleImg.h"

QDlgScaleImg::QDlgScaleImg(int nWidth, int nHeight, QWidget* parent)
	: QDialog(parent)
{
	QDialog::setWindowTitle("Scale Image");
	QDialog::setWindowIcon(QIcon(":/images/btn_scale_img.png"));

	QIntValidator* pValidator = new QIntValidator(this);
	pValidator->setBottom(0);

	m_pEditWidth = new QLineEdit(this);
	m_pEditWidth->setValidator(pValidator);
	m_pEditWidth->setText(QString::number(nWidth));

	m_pEditHeight = new QLineEdit(this);
	m_pEditHeight->setValidator(pValidator);
	m_pEditHeight->setText(QString::number(nHeight));

	QPushButton* pBtnOK = new QPushButton("&OK", this);
	QObject::connect(pBtnOK, SIGNAL(clicked(bool)), this, SLOT(accept()));
	QPushButton* pBtnCancel = new QPushButton("&Cancel", this);
	QObject::connect(pBtnCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QFormLayout* pLayoutForm = new QFormLayout;
	pLayoutForm->addRow("Width", m_pEditWidth);
	pLayoutForm->addRow("Height", m_pEditHeight);

	QHBoxLayout* pLayoutButtons = new QHBoxLayout;
	pLayoutButtons->addStretch();
	pLayoutButtons->addWidget(pBtnOK);
	pLayoutButtons->addWidget(pBtnCancel);

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->addLayout(pLayoutForm);
	pLayoutMain->addLayout(pLayoutButtons);

	QDialog::setLayout(pLayoutMain);
}

void QDlgScaleImg::accept()
{
	m_nWidth = m_pEditWidth->text().toInt();
	m_nHeight = m_pEditHeight->text().toInt();
	QDialog::accept();
}

