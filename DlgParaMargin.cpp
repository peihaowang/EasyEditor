#include "common_headers.h"
#include "DlgParaMargin.h"

QDlgParaMargin::QDlgParaMargin(int nStartIndent, int nMarginLeft, int nMarginRight, int nMarginTop, int nMarginBottom, QWidget* parent)
	: QDialog(parent)
{
	QDialog::setWindowTitle("Paragraph Margin");
	QDialog::setWindowIcon(QIcon(":/images/btn_para_margin.png"));

	m_pEditStartIndent = new QLineEdit(this);
	m_pEditStartIndent->setText(QString::number(nStartIndent));

	m_pEditMarginLeft = new QLineEdit(this);
	m_pEditMarginLeft->setText(QString::number(nMarginLeft));

	m_pEditMarginRight = new QLineEdit(this);
	m_pEditMarginRight->setText(QString::number(nMarginRight));

	m_pEditMarginTop = new QLineEdit(this);
	m_pEditMarginTop->setText(QString::number(nMarginTop));

	m_pEditMarginBottom = new QLineEdit(this);
	m_pEditMarginBottom->setText(QString::number(nMarginBottom));

	QPushButton* pBtnOk = new QPushButton("&OK", this);
	QObject::connect(pBtnOk, SIGNAL(clicked(bool)), this, SLOT(accept()));
	QPushButton* pBtnCancel = new QPushButton("&Cancel", this);
	QObject::connect(pBtnCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QHBoxLayout* pLayoutButtons = new QHBoxLayout;
	pLayoutButtons->addStretch();
	pLayoutButtons->addWidget(pBtnOk);
	pLayoutButtons->addWidget(pBtnCancel);

	QFormLayout* pLayoutForm = new QFormLayout;
	pLayoutForm->addRow("Start Indent", m_pEditStartIndent);
	pLayoutForm->addRow("Margin Left", m_pEditMarginLeft);
	pLayoutForm->addRow("Margin Right", m_pEditMarginRight);
	pLayoutForm->addRow("Margin Top", m_pEditMarginTop);
	pLayoutForm->addRow("Margin Bottom", m_pEditMarginBottom);

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->addLayout(pLayoutForm);
	pLayoutMain->addLayout(pLayoutButtons);

	QDialog::setLayout(pLayoutMain);
}

void QDlgParaMargin::accept()
{
	m_nStartIndent = m_pEditStartIndent->text().toInt();
	m_nMarginLeft = m_pEditMarginLeft->text().toInt();
	m_nMarginRight = m_pEditMarginRight->text().toInt();
	m_nMarginTop = m_pEditMarginTop->text().toInt();
	m_nMarginBottom = m_pEditMarginBottom->text().toInt();
	QDialog::accept();
}
