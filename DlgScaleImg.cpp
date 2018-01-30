
#include "common_headers.h"
#include "DlgScaleImg.h"

QDlgScaleImg::QDlgScaleImg(int nWidth, int nHeight, QWidget* parent)
	: QDialog(parent)
	, m_nOriginWidth(nWidth)
	, m_nOriginHeight(nHeight)
	, m_bBlockChangeSignals(false)
{
	QDialog::setWindowTitle("Scale Image");
	QDialog::setWindowIcon(QIcon(":/images/btn_scale_img.png"));

	QIntValidator* pValidator = new QIntValidator(this);
	pValidator->setBottom(0);

	m_pEditWidth = new QLineEdit(this);
	m_pEditWidth->setValidator(pValidator);
	m_pEditWidth->setText(QString::number(nWidth));
	QObject::connect(m_pEditWidth, SIGNAL(textChanged(QString)), this, SLOT(onWidthChanged(QString)));

	m_pEditHeight = new QLineEdit(this);
	m_pEditHeight->setValidator(pValidator);
	m_pEditHeight->setText(QString::number(nHeight));
	QObject::connect(m_pEditHeight, SIGNAL(textChanged(QString)), this, SLOT(onHeightChanged(QString)));

	m_pChkKeepRatio = new QCheckBox("Keep Aspect Ratio", this);
	m_pChkKeepRatio->setChecked(g_xOpt.m_bDlgScale_KeepAspectRatio);

	QPushButton* pBtnOK = new QPushButton("&OK", this);
	QObject::connect(pBtnOK, SIGNAL(clicked(bool)), this, SLOT(accept()));
	QPushButton* pBtnCancel = new QPushButton("&Cancel", this);
	QObject::connect(pBtnCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QFormLayout* pLayoutForm = new QFormLayout;
	pLayoutForm->addRow("Width", m_pEditWidth);
	pLayoutForm->addRow("Height", m_pEditHeight);
	pLayoutForm->addWidget(m_pChkKeepRatio);

	QHBoxLayout* pLayoutButtons = new QHBoxLayout;
	pLayoutButtons->addStretch();
	pLayoutButtons->addWidget(pBtnOK);
	pLayoutButtons->addWidget(pBtnCancel);

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->addLayout(pLayoutForm);
	pLayoutMain->addLayout(pLayoutButtons);

	QDialog::setLayout(pLayoutMain);
}

void QDlgScaleImg::onWidthChanged(const QString& sText)
{
	if(!m_bBlockChangeSignals){
		int nWidth = sText.toInt();
		if(m_pChkKeepRatio->isChecked()){
			int nHeight = (int)(((double)m_nOriginHeight / (double)m_nOriginWidth) * (double)nWidth);
			_CTempValueChange<bool> _tvc(m_bBlockChangeSignals, true); Q_UNUSED(_tvc);
			m_pEditHeight->setText(QString::number(nHeight));
		}
	}
}

void QDlgScaleImg::onHeightChanged(const QString& sText)
{
	if(!m_bBlockChangeSignals){
		int nHeight = sText.toInt();
		if(m_pChkKeepRatio->isChecked()){
			int nWidth = (int)(((double)m_nOriginWidth / (double)m_nOriginHeight) * (double)nHeight);
			_CTempValueChange<bool> _tvc(m_bBlockChangeSignals, true); Q_UNUSED(_tvc);
			m_pEditWidth->setText(QString::number(nWidth));
		}
	}
}

void QDlgScaleImg::accept()
{
	m_nWidth = m_pEditWidth->text().toInt();
	m_nHeight = m_pEditHeight->text().toInt();
	g_xOpt.m_bDlgScale_KeepAspectRatio = m_pChkKeepRatio->isChecked();
	QDialog::accept();
}

