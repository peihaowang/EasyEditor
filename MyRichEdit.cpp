#include <math.h>
#include <QPrinter>
#include "common_headers.h"

#include "MyRichEdit.h"

///////////////////////////////////////////////////////////

_CMyRichEdit::_CMyRichEdit(QWidget* parent)
	: QTextEdit(parent)
	, m_iListStyle(QTextListFormat::ListStyleUndefined)
	, m_bImageSelected(false)
	, m_bTableSelected(false)
{
	m_pTextDocument = new _CMyDocument(this);
	QTextEdit::setDocument(m_pTextDocument);

	m_pHighlighterOccurrence = new _CHighlighterOccurrence(m_pTextDocument);

	{
		QPalette xPal = QTextEdit::palette();
		QColor clHighlight = xPal.color(QPalette::Active, QPalette::Highlight);
		xPal.setColor(QPalette::Inactive, QPalette::Highlight, clHighlight);
		QTextEdit::setPalette(xPal);
	}

	QTextEdit::setAcceptRichText(false);
	QTextEdit::setMouseTracking(true);
	QTextEdit::viewport()->installEventFilter(this);

	QObject::connect(this->document(), SIGNAL(contentsChanged()), this, SLOT(onDetectBlockFmtChanges()));
	QObject::connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onDetectBlockFmtChanges()));

	QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(onDetectImageSelectionChanged()));

	QObject::connect(this->document(), SIGNAL(contentsChanged()), this, SLOT(onDetectCellsSelectionChanges()));
	QObject::connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onDetectCellsSelectionChanges()));
	QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(onDetectCellsSelectionChanges()));

	QObject::connect(this->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));
	QObject::connect(this->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onContentsChange(int,int,int)));
	QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

void _CMyRichEdit::onDetectBlockFmtChanges()
{
	bool bChanged = false;
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	QTextListFormat::Style iStyle = blockTextListStyle();

	if(m_xLastBlockFmt != xFmt){
		m_xLastBlockFmt = xFmt;
		bChanged = true;
	}
	if(m_iListStyle != iStyle){
		m_iListStyle = iStyle;
		bChanged = true;
	}

	if(bChanged){
		emit currentBlockFormatChanged(xFmt, iStyle);
	}
}

void _CMyRichEdit::onDetectCellsSelectionChanges()
{
	int nRow, nRowCount, nCol, nColCount;
	selectedTableCells(&nRow, &nRowCount, &nCol, &nColCount);
	if(nRow < 0 || nCol < 0 || nRowCount < 0 || nColCount < 0){
		if(m_bTableSelected){
			emit cellsSelectionChanged(-1, -1, -1 ,-1);
			m_bTableSelected = false;
		}
	}else{
		if(!m_bTableSelected) m_bTableSelected = true;
		emit cellsSelectionChanged(nRow, nRowCount, nCol, nColCount);
	}
//	QTextTable* pTable = QTextEdit::textCursor().currentTable();
//	if(pTable){
//		if(QTextEdit::textCursor().hasComplexSelection()){
//			QTextEdit::textCursor().selectedTableCells(&nRow, &nRowCount, &nCol, &nColCount);
//		}else{
//			QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().position());
//			nRow = xCell.row(); nCol = xCell.column(); nRowCount = 1; nColCount = 1;
//		}

//		emit cellsSelectionChanged(nRow, nRowCount, nCol, nColCount);
//		if(!m_bTableSelected) m_bTableSelected = true;
//	}else{
//		if(m_bTableSelected){
//			emit cellsSelectionChanged(-1, -1, -1 ,-1);
//			m_bTableSelected = false;
//		}
//	}
}

void _CMyRichEdit::onDetectImageSelectionChanged()
{
	_CTextImage xTextImage; bool bImageSelected = currentImage();
	if(m_bImageSelected != bImageSelected){
		m_bImageSelected = bImageSelected;
		emit imageSelectionChanged(m_bImageSelected);
	}
}

void _CMyRichEdit::open(const QString& sFn)
{
	if(QFileInfo(sFn).exists()){
		QString sHtml = _CTextFile::load(sFn, "html");
		m_pTextDocument->loadDocument(sHtml);
//		QTextEdit::setHtml(sHtml);
		m_sFilePath = sFn;
	}
}

void _CMyRichEdit::save()
{
	if(!m_sFilePath.isEmpty()){
		saveAs(m_sFilePath);
		setDirty(false);
	}
}

void _CMyRichEdit::saveAs(const QString& sFn) const
{
//	QString sHtml = QTextEdit::toHtml();
	m_pTextDocument->cleanUnlinkedImages();
	QString sContent = m_pTextDocument->content();
	_CTextFile::saveUtf8(sFn, sContent, true);
}

void _CMyRichEdit::exportAsHtml(const QString& sFn) const
{
	m_pTextDocument->cleanUnlinkedImages();
	QString sHtml = m_pTextDocument->convertToHtml();
	_CTextFile::saveUtf8(sFn, sHtml, true);
}

void _CMyRichEdit::exportAsPdf(const QString& sFn) const
{
	//2018.2.4 Printer mode;
	QPrinter::PrinterMode iMode=QPrinter::HighResolution;
	//2018.2.4 Output format;
	QPrinter::OutputFormat iFmt=QPrinter::PdfFormat;
	//2018.2.4 Paper size;
	QPrinter::PaperSize iPaper=QPrinter::A4;

	QPrinter xPrinter(iMode);
	xPrinter.setOutputFileName(sFn);
	xPrinter.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
	xPrinter.setOutputFormat(iFmt);
	xPrinter.setPaperSize(iPaper);

	m_pTextDocument->print(&xPrinter);
}

void _CMyRichEdit::exportAsPlainText(const QString &sFn) const
{
	m_pTextDocument->cleanUnlinkedImages();
	QString sText = m_pTextDocument->toPlainText();
	_CTextFile::saveUtf8(sFn, sText, true);
}

void _CMyRichEdit::exportSelectedImage(const QString &sFn) const
{
	_CTextImage* pImage = currentImage();
	if(pImage){
		QPixmap xImage = pImage->transformedPixmap();
		xImage.save(sFn);
	}
}

bool _CMyRichEdit::fontBold() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return (xFmt.fontWeight() == QFont::Bold);
}

void _CMyRichEdit::setFontBold(bool b)
{
	QTextCharFormat xFmt; xFmt.setFontWeight(b ? QFont::Bold : QFont::Normal);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

QTextCharFormat::UnderlineStyle _CMyRichEdit::fontUnderlineStyle() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.underlineStyle();
}

void _CMyRichEdit::setFontUnderlineStyle(QTextCharFormat::UnderlineStyle iStyle)
{
	QTextCharFormat xFmt; xFmt.setUnderlineStyle(iStyle);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

QColor _CMyRichEdit::fontUnderlineColor() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.underlineColor();
}

void _CMyRichEdit::setFontUnderlineColor(const QColor& clUnderline)
{
	QTextCharFormat xFmt; xFmt.setFontUnderline(true); xFmt.setUnderlineColor(clUnderline);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

bool _CMyRichEdit::fontOverline() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.fontOverline();
}

void _CMyRichEdit::setFontOverline(bool b)
{
	QTextCharFormat xFmt; xFmt.setFontOverline(b);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

bool _CMyRichEdit::fontStrikeOut() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.fontStrikeOut();
}

void _CMyRichEdit::setFontStrikeOut(bool b)
{
	QTextCharFormat xFmt; xFmt.setFontStrikeOut(b);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

bool _CMyRichEdit::fontSupersript() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return (xFmt.verticalAlignment() == QTextCharFormat::AlignSuperScript);
}

void _CMyRichEdit::setFontSupersript(bool b)
{
	QTextCharFormat xFmt; xFmt.setVerticalAlignment(b ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

bool _CMyRichEdit::fontSubsript() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return (xFmt.verticalAlignment() == QTextCharFormat::AlignSubScript);
}

void _CMyRichEdit::setFontSubsript(bool b)
{
	QTextCharFormat xFmt; xFmt.setVerticalAlignment(b ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

QBrush _CMyRichEdit::textForeground() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.foreground();
}

void _CMyRichEdit::setTextForeground(const QBrush& brush)
{
	QTextCharFormat xFmt; xFmt.setForeground(brush);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

QBrush _CMyRichEdit::textBackground() const
{
	QTextCharFormat xFmt = QTextEdit::textCursor().charFormat();
	return xFmt.background();
}

void _CMyRichEdit::setTextBackground(const QBrush& brush)
{
	QTextCharFormat xFmt; xFmt.setBackground(brush);
	QTextEdit::mergeCurrentCharFormat(xFmt);
}

QTextBlockFormat _CMyRichEdit::currentBlockFormat() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt;
}

QList<QTextBlock> _CMyRichEdit::selectedBlocks() const
{
	QList<QTextBlock> vBlocks;
	int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);
	if(nBegin >= 0 && nEnd >= 0){
		QTextCursor xCursorBegin = textCursorOfPosition(nBegin);
		QTextCursor xCursorEnd = textCursorOfPosition(nEnd);

		QTextBlock xBlock = xCursorBegin.block();
		while(true){
			vBlocks << xBlock;
			if(xBlock != xCursorEnd.block() && xBlock.next().isValid()){
				xBlock = xBlock.next();
			}else{
				break;
			}
		}
	}
	return vBlocks;
}

Qt::Alignment _CMyRichEdit::blockAlignment() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.alignment();
}

void _CMyRichEdit::setBlockAlignment(Qt::Alignment nAlignment)
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setAlignment(nAlignment);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

int _CMyRichEdit::blockIndent() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.indent();
}

void _CMyRichEdit::setBlockIndent(int nIndent)
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setIndent(nIndent > 0 ? nIndent : 0);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::blockTextIndent() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.textIndent();
}

void _CMyRichEdit::setBlockTextIndent(qreal nIndent)
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setTextIndent(nIndent);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::blockLeftMargin() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.leftMargin();
}

void _CMyRichEdit::setBlockLeftMargin(qreal nMargin) const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setLeftMargin(nMargin);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::blockRightMargin() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.rightMargin();
}

void _CMyRichEdit::setBlockRightMargin(qreal nMargin) const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setRightMargin(nMargin);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::blockTopMargin() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.topMargin();
}

void _CMyRichEdit::setBlockTopMargin(qreal nMargin) const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setTopMargin(nMargin);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::blockBottomMargin() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return xFmt.bottomMargin();
}

void _CMyRichEdit::setBlockBottomMargin(qreal nMargin) const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setBottomMargin(nMargin);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::lineHeight() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return (xFmt.lineHeightType() == QTextBlockFormat::FixedHeight) ? xFmt.lineHeight() : -1.0;
}

void _CMyRichEdit::setLineSpacing(qreal nSpacing)
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setLineHeight(nSpacing, QTextBlockFormat::FixedHeight);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

qreal _CMyRichEdit::lineHeightProportional() const
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	return (xFmt.lineHeightType() == QTextBlockFormat::ProportionalHeight) ? xFmt.lineHeight() : -1.0;
}

void _CMyRichEdit::setLineSpacingProportional(qreal nProportion)
{
	QTextBlockFormat xFmt = QTextEdit::textCursor().blockFormat();
	xFmt.setLineHeight(nProportion, QTextBlockFormat::ProportionalHeight);
	QTextEdit::textCursor().mergeBlockFormat(xFmt);
}

bool _CMyRichEdit::isLineSpacingx1() const {return (lineHeightProportional() == 100.0);}

void _CMyRichEdit::setLineSpacingx1(){setLineSpacingProportional(100.0);}

bool _CMyRichEdit::isLineSpacingx15() const {return (lineHeightProportional() == 150.0);}

void _CMyRichEdit::setLineSpacingx15() {setLineSpacingProportional(150.0);}

bool _CMyRichEdit::isLineSpacingx2() const {return (lineHeightProportional() == 200.0);}

void _CMyRichEdit::setLineSpacingx2() {setLineSpacingProportional(200.0);}

void _CMyRichEdit::removeBlockFromTextList(const QTextBlock& xBlock)
{
	QTextList* pTextList = xBlock.textList();
	if(pTextList){
		pTextList->remove(xBlock);

		QTextCursor xTC = textCursorOfBlock(xBlock, false);
		QTextBlockFormat xFmt = xBlock.blockFormat(); xFmt.setIndent(xFmt.indent() - 1);
		xTC.mergeBlockFormat(xFmt);
	}
}

void _CMyRichEdit::setBlockTextList(QTextListFormat::Style iStyle)
{
	beginEdit();
	QTextEdit::textCursor().createList(iStyle);
	endEdit();
}

void _CMyRichEdit::cancelBlockTextList()
{
	beginEdit();

	QList<QTextBlock> vBlocks = selectedBlocks();
	Q_FOREACH(QTextBlock xBlock, vBlocks){
		removeBlockFromTextList(xBlock);
	}

	endEdit();
}

bool _CMyRichEdit::blockOrderedList() const
{
	return (blockTextListStyle() == QTextListFormat::ListDecimal);
}

void _CMyRichEdit::setBlockOrderedList(bool bList)
{
	if(bList){
		setBlockTextList(QTextListFormat::ListDecimal);
	}else{
		cancelBlockTextList();
	}
}

void _CMyRichEdit::setBlockOrderedListFmt(const QString& sPrefix, const QString& sSuffix)
{
	beginEdit();

	QList<QTextBlock> vBlocks = selectedBlocks();
	Q_FOREACH(QTextBlock xBlock, vBlocks){
		QTextList* pList = xBlock.textList();
		if(pList){
			QTextListFormat xFmt = pList->format();
			xFmt.setNumberPrefix(sPrefix);
			xFmt.setNumberSuffix(sSuffix);
			pList->setFormat(xFmt);
		}
	}

	endEdit();
}

bool _CMyRichEdit::blockUnorderedList() const
{
	return (blockTextListStyle() == QTextListFormat::ListDisc);
}

void _CMyRichEdit::setBlockUnorderedList(bool bList)
{
	if(bList){
		setBlockTextList(QTextListFormat::ListDisc);
	}else{
		cancelBlockTextList();
	}
}

void _CMyRichEdit::demoteAsChildList()
{
	beginEdit();

	QTextListFormat::Style iStyle = blockTextListStyle();
	if(iStyle == QTextListFormat::ListStyleUndefined){
		int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);

		QTextBlock xBlockPrev = textCursorOfPosition(nBegin).block().previous();
		QTextList* pTextList = xBlockPrev.textList();
		if(pTextList){
			iStyle = pTextList->format().style();
		}
	}

	if(iStyle == QTextListFormat::ListStyleUndefined){
		QTextListFormat::Style iDefStyle = QTextListFormat::ListDecimal;
		iStyle = iDefStyle;
	}

	indent(1);
	setBlockTextList(iStyle);

	endEdit();
}

void _CMyRichEdit::promoteToParentList()
{
	int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);
	QTextBlock xBlockCurrent = textCursorOfPosition(nBegin).block();
	if(xBlockCurrent.textList()){
		beginEdit();

		QTextBlock xBlockPrev = xBlockCurrent;
		do{
			xBlockPrev = xBlockPrev.previous();
		}while(xBlockPrev.isValid() && xBlockPrev.textList() == xBlockCurrent.textList());

		QTextList* pTextList = xBlockPrev.textList();
		if(pTextList){
			QList<QTextBlock> v = selectedBlocks();
			Q_FOREACH(QTextBlock xBlock, v){
				if(xBlock.textList()){
					xBlock.textList()->remove(xBlock);
				}
				pTextList->add(xBlock);

				{
					QTextCursor xTC = textCursorOfBlock(xBlock, false);
					QTextBlockFormat xFmt = xBlock.blockFormat();
					xFmt.setIndent(xBlockPrev.blockFormat().indent());
					xTC.mergeBlockFormat(xFmt);
				}
			}
		}

		endEdit();
	}
}

_CTextImage* _CMyRichEdit::currentImage() const
{
	_CTextImage* pTextImage = NULL;
	QTextImageFormat xFmt; QTextCursor xTC = QTextEdit::textCursor();
	if(hasSelection() && qAbs(xTC.anchor() - xTC.position()) == 1){
		QTextCursor xTCAnchor(m_pTextDocument); xTCAnchor.setPosition(QTextEdit::textCursor().anchor(), QTextCursor::MoveAnchor);
		QTextCursor xTCPosition(m_pTextDocument); xTCPosition.setPosition(QTextEdit::textCursor().position(), QTextCursor::MoveAnchor);

		if(xTC.anchor() > xTC.position() && xTCAnchor.charFormat().isImageFormat()){
			xFmt = xTCAnchor.charFormat().toImageFormat();
		}else if(xTC.anchor() < xTC.position() && xTCPosition.charFormat().isImageFormat()){
			xFmt = xTCPosition.charFormat().toImageFormat();
		}
	}
	if(!xFmt.isEmpty()){
		pTextImage = m_pTextDocument->imageOf(QUrl(xFmt.name()));
	}
	return pTextImage;
}

void _CMyRichEdit::insertImage(const QPixmap& xImg)
{
	_CTextImage xTextImg(xImg);
	m_pTextDocument->beginMacro("Insert Image");

	_CUndoCmdInsertImage* pUndoCmd = new _CUndoCmdInsertImage(m_pTextDocument, xTextImg);
	m_pTextDocument->pushToUndoStack(pUndoCmd);
	QTextEdit::textCursor().insertImage(xTextImg.constructTextImageFmt());
//	QTextEdit::textCursor().insertHtml(xTextImg.imageTagStr());

	m_pTextDocument->endMacro();
}

int _CMyRichEdit::imageRotation() const
{
	int rotation = -1; _CTextImage* pTextImg = currentImage();
	if(pTextImg){
		rotation = pTextImg->rotation();
	}
	if(rotation < 0) rotation = 0;
	return rotation;
}

void _CMyRichEdit::rotateImage(int nRotation)
{
	_CTextImage* pTextImg = currentImage();
	if(pTextImg){
		m_pTextDocument->beginMacro("Rotate Image");

		_CUndoCmdRotateImage* pUndoCmd = new _CUndoCmdRotateImage(m_pTextDocument, pTextImg->url(), nRotation);
		m_pTextDocument->pushToUndoStack(pUndoCmd);
		QTextEdit::textCursor().mergeCharFormat(pTextImg->constructTextImageFmt());

		m_pTextDocument->endMacro();
	}
}

int _CMyRichEdit::imageWidth() const
{
	int w = -1; _CTextImage* pTextImg = currentImage();
	if(pTextImg){
		w = (int)((double)pTextImg->sourcePixmap().width() * pTextImg->scaleValX());
//		w = m_pTextDocument->sourceImage(xTextImg, true).width();
	}
	return w;
}

int _CMyRichEdit::imageHeight() const
{
	int h = -1; _CTextImage* pTextImg = currentImage();
	if(pTextImg){
		h = (int)((double)pTextImg->sourcePixmap().height() * pTextImg->scaleValY());
//		h = m_pTextDocument->sourceImage(xTextImg, true).height();
	}
	return h;
}

void _CMyRichEdit::scaleImage(qreal x, qreal y)
{
	_CTextImage* pTextImg = currentImage();
	if(pTextImg){
		m_pTextDocument->beginMacro("Scale Image");

		_CUndoCmdScaleImage* pUndoCmd = new _CUndoCmdScaleImage(m_pTextDocument, pTextImg->url(), x, y);
		m_pTextDocument->pushToUndoStack(pUndoCmd);
		QTextEdit::textCursor().mergeCharFormat(pTextImg->constructTextImageFmt());

		m_pTextDocument->endMacro();
	}
}

void _CMyRichEdit::resizeImageTo(int width, int height)
{
	_CTextImage* pTextImg = currentImage();
	if(pTextImg){
		QPixmap xSrcPixmap = pTextImg->sourcePixmap();
		double x = (double)width / (double)xSrcPixmap.width(), y = (double)height / (double)xSrcPixmap.height();
		scaleImage(x, y);
	}
}

void _CMyRichEdit::selectedTableCells(int* rowFirst, int* rowCount, int* colFirst, int* colCount) const
{
	int rowFirst0 = -1, rowCount0 = -1, colFirst0 = -1, colCount0 = -1;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		if(QTextEdit::textCursor().hasComplexSelection()){
			QTextEdit::textCursor().selectedTableCells(&rowFirst0, &rowCount0, &colFirst0, &colCount0);
		}else{
			QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().position());
			rowFirst0 = xCell.row(); colFirst0 = xCell.column(); rowCount0 = 1; colCount0 = 1;
		}
	}

	if(rowFirst) (*rowFirst) = rowFirst0;
	if(rowCount) (*rowCount) = rowCount0;
	if(colFirst) (*colFirst) = colFirst0;
	if(colCount) (*colCount) = colCount0;
}

void _CMyRichEdit::insertTable(int row, int col)
{
	QTextTableFormat xFmt;
	xFmt.setCellPadding(3); xFmt.setCellSpacing(0);
	xFmt.setBorder(1.0);
	xFmt.setBorderStyle(QTextTableFormat::BorderStyle_Solid);
	xFmt.setBorderBrush(QBrush(Qt::gray));

	{
		QVector<QTextLength> vConstraints; const int nColWidth = 100;
		for(int i = 0; i < col; i++){
			vConstraints.push_back(QTextLength(QTextLength::FixedLength, nColWidth));
		}
		xFmt.setColumnWidthConstraints(vConstraints);
	}

	QTextEdit::textCursor().insertTable(row, col, xFmt);
}

void _CMyRichEdit::insertRows(int row, int num)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		if(row >= 0){
			if(row >= pTable->rows()){
				pTable->appendRows(num);
			}else{
				pTable->insertRows(row, num);
			}
		}
	}
}

void _CMyRichEdit::insertRowBefore()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().selectionStart());
		if(xCell.isValid()){
			insertRows(xCell.row(), 1);
		}
	}
}

void _CMyRichEdit::insertRowAfter()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().selectionEnd());
		if(xCell.isValid()){
			int nOffset = xCell.rowSpan();
			insertRows(xCell.row() + nOffset, 1);
		}
	}
}

void _CMyRichEdit::insertCols(int col, int num)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		if(col >= 0){
			if(col >= pTable->columns()){
				pTable->appendColumns(num);
			}else{
				pTable->insertColumns(col, num);
			}
		}
	}
}

void _CMyRichEdit::insertColBefore()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().selectionStart());
		if(xCell.isValid()){
			insertCols(xCell.column(), 1);
		}
	}
}

void _CMyRichEdit::insertColAfter()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(QTextEdit::textCursor().selectionEnd());
		if(xCell.isValid()){
			int nOffset = xCell.columnSpan();
			insertCols(xCell.column() + nOffset, 1);
		}
	}
}

void _CMyRichEdit::removeRows(int row, int num)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		pTable->removeRows(row, num);
	}
}

void _CMyRichEdit::removeCols(int col, int num)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		pTable->removeColumns(col, num);
	}
}

void _CMyRichEdit::removeRows()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		int nRow = -1, nRowCount = 0;
		selectedTableCells(&nRow, &nRowCount, NULL, NULL);
//		if(QTextEdit::textCursor().hasComplexSelection()){
//			int nCol, nColCount; QTextEdit::textCursor().selectedTableCells(&nRow, &nRowCount, &nCol, &nColCount);
//		}else{
//			QTextTableCell xCurCell = pTable->cellAt(QTextEdit::textCursor().position());
//			nRow = xCurCell.row();
//			nRowCount = 1;
//		}

		if(nRow >= 0 && nRowCount >= 1){
			removeRows(nRow, nRowCount);
		}
	}
}

void _CMyRichEdit::removeCols()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		int nCol = -1, nColCount = 0;
		selectedTableCells(NULL, NULL, &nCol, &nColCount);
//		if(QTextEdit::textCursor().hasComplexSelection()){
//			int nRow, nRowCount; QTextEdit::textCursor().selectedTableCells(&nRow, &nRowCount, &nCol, &nColCount);
//		}else{
//			QTextTableCell xCurCell = pTable->cellAt(QTextEdit::textCursor().position());
//			nCol = xCurCell.column();
//			nColCount = 1;
//		}

		if(nCol >= 0 && nColCount >= 1){
			removeCols(nCol, nColCount);
		}
	}
}

void _CMyRichEdit::mergeTableCells()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		pTable->mergeCells(QTextEdit::textCursor());
	}
}

void _CMyRichEdit::splitTableCells()
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		beginEdit();
		_CPredGetMergedCell xPred; traverseSelectedCells(xPred);
		Q_FOREACH(QTextTableCell xCell, xPred.m_vMergedCells){
			pTable->splitCell(xCell.row(), xCell.column(), 1, 1);
		}
		endEdit();
	}
}

QColor _CMyRichEdit::tableBorderColor() const
{
	QColor clPen;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		clPen = xFmt.borderBrush().color();
	}
	return clPen;
}

void _CMyRichEdit::setTableBorderColor(const QColor& pen)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		xFmt.setBorderBrush(brushOfColor(pen));
		pTable->setFormat(xFmt);
	}
}

QColor _CMyRichEdit::tableBackgroundColor() const
{
	QColor clBrush;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		clBrush = xFmt.background().color();
	}
	return clBrush;
}

void _CMyRichEdit::setTableBackgroundColor(const QColor& brush)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		xFmt.setBackground(brushOfColor(brush));
		pTable->setFormat(xFmt);
	}
}

QColor _CMyRichEdit::cellBackgroundColor(int row, int col) const
{
	QColor clBrush;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(row, col);
		QTextCharFormat xFmt = xCell.format();
		clBrush = xFmt.background().color();
	}
	return clBrush;
}

void _CMyRichEdit::setCellBackgroundColor(int row, int col, const QColor& clBrush)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableCell xCell = pTable->cellAt(row, col);
		QTextCharFormat xFmt; xFmt.setBackground(brushOfColor(clBrush));
		xCell.firstCursorPosition().mergeBlockCharFormat(xFmt);
	}
}

void _CMyRichEdit::setCellsBackgroundColor(const QColor& brush)
{
	beginEdit();
	_CPredSetCellBackground xPred(brush);
	traverseSelectedCells(xPred);
	endEdit();
}

int _CMyRichEdit::colWidthConstraints(int col) const
{
	int iWidth = -1;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		QVector<QTextLength> vConstraints = xFmt.columnWidthConstraints();
		if(col >= 0 && col < vConstraints.size()){
			QTextLength xLen = vConstraints[col];
			if(xLen.type() == QTextLength::FixedLength) iWidth = xLen.rawValue();
		}
		pTable->setFormat(xFmt);
	}
	return iWidth;
}

void _CMyRichEdit::setColWidthConstraints(int col, int iWidth)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		QVector<QTextLength> vConstraints = xFmt.columnWidthConstraints();
		if(col >= 0 && col < vConstraints.size()){
			vConstraints[col] = QTextLength(QTextLength::FixedLength, iWidth >= 0 ? iWidth : 0);
		}
		xFmt.setColumnWidthConstraints(vConstraints);
		pTable->setFormat(xFmt);
	}
}

void _CMyRichEdit::setColsWidthConstraints(int width)
{
	beginEdit();
	_CPredSetColWidthConstraints xPred(_CPredSetColWidthConstraints::FixedValue, width);
	traverseSelectedCells(xPred);
	endEdit();
}

void _CMyRichEdit::increaseColsWidthConstraints(int increasement)
{
	beginEdit();
	_CPredSetColWidthConstraints xPred(_CPredSetColWidthConstraints::Increasement, increasement);
	traverseSelectedCells(xPred);
	endEdit();
}

Qt::Alignment _CMyRichEdit::tableAlignment() const
{
	Qt::Alignment nAlignment = Qt::AlignLeft;
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		nAlignment = xFmt.alignment();
	}
	return nAlignment;
}

void _CMyRichEdit::setTableAlignment(Qt::Alignment nAlignment)
{
	QTextTable* pTable = QTextEdit::textCursor().currentTable();
	if(pTable){
		QTextTableFormat xFmt = pTable->format();
		xFmt.setAlignment(nAlignment);
		pTable->setFormat(xFmt);
	}
}

//void _CMyRichEdit::setTextFrame(qreal nWidth, QTextFrameFormat::BorderStyle iStyle, const QColor& clColor)
//{
//	beginEdit();
//	QTextFrame* pFrame = QTextEdit::textCursor().currentFrame();
////	if(pFrame != m_pTextDocument->rootFrame()){
////		pFrame->setFrameFormat(QTextFrameFormat());
////	}else{
//		QTextFrameFormat xFmt;
//		xFmt.setBorder(nWidth);
//		xFmt.setBorderBrush(QBrush(clColor));
//		xFmt.setBorderStyle(iStyle);

//		int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);
//		if(nBegin >= 0 && nEnd >= 0){
//			QTextBlock xBlockBegin = textCursorOfPosition(nBegin).block();
//			QTextBlock xBlockEnd = textCursorOfPosition(nEnd).block();
//			QTextCursor xTC(m_pTextDocument);
//			xTC.setPosition(xBlockBegin.position(), QTextCursor::MoveAnchor);
//			xTC.setPosition(xBlockEnd.position() + xBlockEnd.length() - 1, QTextCursor::KeepAnchor);
//			xTC.insertFrame(xFmt);
//			{
//				QTextCursor xToRm = textCursorOfBlock(xBlockEnd.next(), true);
//				xToRm.removeSelectedText();
//			}
//			{
//				QTextCursor xToRm = textCursorOfBlock(xBlockBegin, true);
////				xToRm.removeSelectedText();
//				QTextEdit::setTextCursor(xToRm);
//			}
//		}
////	}
//		endEdit();
//}

//void _CMyRichEdit::removeTextFrame()
//{
//	QTextFrame* pFrame = QTextEdit::textCursor().currentFrame();
//	if(pFrame){
//		pFrame->deleteLater();
//	}
//}

void _CMyRichEdit::insertUrl(const QString& sTitle, const QString& sUrl)
{
	QString sHtml = QString("<a href=\"%1\">%2</a>").arg(sUrl).arg(sTitle);
	QTextEdit::textCursor().insertFragment(QTextDocumentFragment::fromHtml(sHtml));
}

void _CMyRichEdit::setUrlForSelectedText(const QString& sUrl)
{
	if(hasSelection()){
		QTextCharFormat xFmt;
		xFmt.setAnchor(true);
		xFmt.setAnchorHref(sUrl);
		xFmt.setForeground(QBrush(Qt::blue));
		xFmt.setFontUnderline(true);
		QTextEdit::mergeCurrentCharFormat(xFmt);
	}
}

void _CMyRichEdit::clearUrlForSelectedText()
{
	if(hasSelection()){
		QTextCharFormat xFmt;
		xFmt.setAnchor(false);
		xFmt.setAnchorHref("");
		QTextEdit::mergeCurrentCharFormat(xFmt);
	}
}

void _CMyRichEdit::highlightOccurrences(const QString& sText, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp)
{
	m_pHighlighterOccurrence->setCaseSensitivity(bCaseSensitive);
	m_pHighlighterOccurrence->setMatchWholeWords(bMatchWholeWord);
	m_pHighlighterOccurrence->setUseRegExp(bUseRegExp);
	m_pHighlighterOccurrence->setText2Find(sText);
	m_pHighlighterOccurrence->rehighlight();
}

bool _CMyRichEdit::findNext(const QString& sText, bool bWrapsAround, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp, bool bBackward)
{
	bool bFound = false;
	QTextDocument::FindFlags nFlags = (QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords | QTextDocument::FindBackward);
	if(!bCaseSensitive) nFlags &= ~QTextDocument::FindCaseSensitively;
	if(!bMatchWholeWord) nFlags &= ~QTextDocument::FindWholeWords;
	if(!bBackward) nFlags &= ~QTextDocument::FindBackward;

	QRegExp xExp(bUseRegExp ? sText : QRegExp::escape(sText));
	QTextCursor xTC = m_pTextDocument->find(xExp, QTextEdit::textCursor(), nFlags);

	if(xTC.isNull() && bWrapsAround){
		int nWrapPos = bBackward ? length() : 0;
		xTC = m_pTextDocument->find(sText, textCursorOfPosition(nWrapPos), nFlags);
	}

	if(!xTC.isNull()){
		QTextEdit::setTextCursor(xTC);
		bFound = true;
	}else{
		bFound = false;
	}
	return bFound;
}

bool _CMyRichEdit::replaceText(const QString& sText, const QString& sReplaceWith, bool bWrapsAround, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp)
{
	QString sSelectedText = QTextEdit::textCursor().selectedText();
	if(!sSelectedText.isEmpty() && sSelectedText.compare(sText, (bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) == 0){
		QTextEdit::textCursor().insertText(sReplaceWith);
	}

	return findNext(sText, bWrapsAround, bCaseSensitive, bMatchWholeWord, bUseRegExp, false);
}

int _CMyRichEdit::replaceAll(const QString& sText, const QString& sReplaceWith, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp)
{
	QTextEdit::moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);

	int nTimes = 0;
	bool bFound = true;
	beginEdit();
	do{
		bFound = findNext(sText, false, bCaseSensitive, bMatchWholeWord, bUseRegExp, false);
		if(bFound){
			QString sSelectedText = QTextEdit::textCursor().selectedText();
			if(!sSelectedText.isEmpty() && sSelectedText.compare(sText, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0){
				QTextEdit::textCursor().insertText(sReplaceWith);
				nTimes++;
			}
		}
	}while(bFound);
	endEdit();
	return nTimes;
}

void _CMyRichEdit::mouseMoveEvent(QMouseEvent* e)
{
	QString sAnchor = QTextEdit::anchorAt(e->pos());
	if(sAnchor.isEmpty()){
		QCursor* pCursor = QApplication::overrideCursor();
		if(pCursor) QApplication::restoreOverrideCursor();
	}else{
		QCursor* pCursor = QApplication::overrideCursor();
		if(!pCursor || (pCursor && pCursor->shape() != Qt::PointingHandCursor)) QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
	}

	if(m_sHyperlinkHovered != sAnchor){
		m_sHyperlinkHovered = sAnchor;
		emit linkHovered(m_sHyperlinkHovered);
	}

	QTextEdit::mouseMoveEvent(e);
}

void _CMyRichEdit::mouseReleaseEvent(QMouseEvent* e)
{
	if(!m_sHyperlinkHovered.isEmpty()){
		if(e->button() == Qt::LeftButton){
			emit linkClicked(m_sHyperlinkHovered);
		}else if(e->button() == Qt::RightButton){
			QTextCursor xTC = QTextEdit::cursorForPosition(e->pos());
			if(xTC.charFormat().anchorHref() != m_sHyperlinkHovered){
				xTC = textCursorOfPosition(xTC.position() + 1);
			}
			int iPos = xTC.position(), nBegin = -1, nEnd = -1;
			{
				while(true){
					QString sHref = textCursorOfPosition(iPos).charFormat().anchorHref();
					if(sHref == m_sHyperlinkHovered){
						iPos--;
					}else{
						break;
					}
				}
				nBegin = iPos;
				if(textCursorOfPosition(nBegin).block() != textCursorOfPosition(nBegin + 1).block()){
					nBegin++;
				}
			}

			{
				iPos = xTC.position();
				while(true){
					QString sHref = textCursorOfPosition(iPos).charFormat().anchorHref();
					if(sHref == m_sHyperlinkHovered){
						iPos++;
					}else{
						break;
					}
				}
				nEnd = iPos - 1;
			}

			QTextCursor xSelection = textCursorOfPosition(nBegin);
			xSelection.setPosition(nEnd, QTextCursor::KeepAnchor);
			QTextEdit::setTextCursor(xSelection);
		}
	}

	QTextEdit::mouseReleaseEvent(e);
}

void _CMyRichEdit::keyPressEvent(QKeyEvent* e)
{
	bool bHandle = false;
	//implement list level
	if(e->key() == Qt::Key_Tab){
		int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);
		QTextListFormat::Style iStyle = blockTextListStyle();
		if(iStyle == QTextListFormat::ListStyleUndefined){
			QTextBlock xBlockPrev = textCursorOfPosition(nBegin).block().previous();
			QTextList* pTextList = xBlockPrev.textList();
			if(pTextList){
				iStyle = pTextList->format().style();
			}
		}
		int nSelLen = nEnd - nBegin;
		int nPosInBlock = QTextEdit::textCursor().positionInBlock();
		if(iStyle != QTextListFormat::ListStyleUndefined && (nSelLen > 0 || nPosInBlock == 0)){
			demoteAsChildList();
			bHandle = true;
		}
	}else if(e->key() == Qt::Key_Backspace){
		int nBegin = -1, nEnd = -1; selection(nBegin, nEnd);
		QTextBlock xBlockCurrent = textCursorOfPosition(nBegin).block();
		int nSelLen = nEnd - nBegin;
		int nPosInBlock = QTextEdit::textCursor().positionInBlock();
		if(xBlockCurrent.textList() && (nSelLen > 0 || nPosInBlock == 0)){
			promoteToParentList();
			bHandle = true;
		}
	}
	if(!bHandle){
		//2018.2.3 reimplement default commands
		if(e->matches(QKeySequence::Undo)){
			_undo();
		}else if(e->matches(QKeySequence::Redo)){
			_redo();
		}else{
			QTextEdit::keyPressEvent(e);
		}
	}
}

QVariant _CMyRichEdit::loadResource(int type, const QUrl& name)
{
//	QVariant xRes;
//	if(type == QTextDocument::ImageResource){
//		_CTextImage xTextImg(name.toString());
//		QPixmap xImg = m_pTextDocument->sourceImage(xTextImg, false);
//		xRes = QVariant(xImg);
//	}else{
//		xRes = QTextEdit::loadResource(type, name);
//	}
//	return xRes;
//	qDebug() << "load" << type << name;
	return QTextEdit::loadResource(type, name);
//	return m_pTextDocument->loadResource(type, name);
}

bool _CMyRichEdit::canInsertFromMimeData(const QMimeData* source) const
{
	bool bAccepted = false;

	if(source->hasImage()){
		bAccepted = true;
	}else{
		bAccepted = QTextEdit::canInsertFromMimeData(source);
	}

	return bAccepted;
}

void _CMyRichEdit::insertFromMimeData(const QMimeData* source)
{
	if(source->hasHtml()){
		QTextEdit::textCursor().insertFragment(QTextDocumentFragment::fromHtml(source->html()));
	}else if(source->hasImage()){
		QImage xImage = source->imageData().value<QImage>();
		insertImage(QPixmap::fromImage(xImage));
	}else{
		QTextEdit::insertFromMimeData(source);
	}
}

//QStringList _CMyRichEdit::_CImageUtils::extractImageUrls(const QString& sHtml)
//{
//	QDomDocument xDomDoc; xDomDoc.setContent(sHtml);
//	if(!xDomDoc.isNull()){
//		;
//	}
//}
