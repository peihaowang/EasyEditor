#ifndef _CMYRICHEDIT_H
#define _CMYRICHEDIT_H

#include <QtDebug>
#include <QUrl>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
#include <QTextTable>
#include <QDomDocument>
#include <QSyntaxHighlighter>

#include "MyDocument.h"

class _CMyRichEdit : public QTextEdit
{

	Q_OBJECT

protected:

	QString					m_sFilePath;

	_CMyDocument *				m_pTextDocument;

	QTextBlockFormat			m_xLastBlockFmt;
	QTextListFormat::Style			m_iListStyle;

	bool					m_bImageSelected;
	bool					m_bTableSelected;

	QString					m_sHyperlinkHovered;

	_CHighlighterOccurrence *		m_pHighlighterOccurrence;

protected:

	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* e);

	virtual void keyPressEvent(QKeyEvent* e);

	virtual QVariant loadResource(int type, const QUrl& name);

	virtual bool canInsertFromMimeData(const QMimeData *source) const;
	virtual void insertFromMimeData(const QMimeData *source);

	//2016.12.3 construct a brush containing the specified color
	//if the color is transparent, the brush will be set to NoBrush
	QBrush brushOfColor(const QColor& clColor) const
	{
		QBrush xBrush = (clColor.alpha() == 0) ? QBrush(Qt::NoBrush) : QBrush(clColor);
		return xBrush;
	}

	QTextCursor textCursorOfPosition(int iPosition) const
	{
		QTextCursor xTC(QTextEdit::document()); xTC.setPosition(iPosition, QTextCursor::MoveAnchor);
		return xTC;
	}

	QTextCursor textCursorOfBlock(const QTextBlock& xBlock, bool bSelectBlock) const
	{
		QTextCursor xTC = textCursorOfPosition(xBlock.position());
		if(bSelectBlock) xTC.setPosition(xTC.anchor() + xBlock.length(), QTextCursor::KeepAnchor);
		return xTC;
	}

	QList<QTextBlock> selectedBlocks() const;

	_CTextImage* currentImage() const;

	//2017.2.10 for const function
	template<typename _CPred> void traverseSelectedBlocks(_CPred& xPred) const
	{
		QList<QTextBlock> vBlocks = selectedBlocks();
		Q_FOREACH(QTextBlock xBlock, vBlocks){
			bool bBreak = false;
			xPred(this, xBlock, bBreak);
			if(bBreak) break;
		}
	}

	class _CPredIndentBlocks
	{
	protected:
		int		m_nLength;
	public:

		_CPredIndentBlocks(int nLength = 1)
			: m_nLength(nLength)
		{return;}

		void operator()(const _CMyRichEdit* pEdit, const QTextBlock& xBlock, bool& bBreak)
		{
			Q_UNUSED(bBreak);
			QTextBlockFormat xFmt = xBlock.blockFormat(); xFmt.setIndent(xFmt.indent() + m_nLength);
			QTextCursor xTC = pEdit->textCursorOfBlock(xBlock, false);
			xTC.mergeBlockFormat(xFmt);
		}
	};

	class _CPredOutdentBlocks
	{
	protected:
		int		m_nLength;
	public:

		_CPredOutdentBlocks(int nLength = 1)
			: m_nLength(nLength)
		{return;}

		void operator()(const _CMyRichEdit* pEdit, const QTextBlock& xBlock, bool& bBreak)
		{
			Q_UNUSED(bBreak);
			QTextBlockFormat xFmt = xBlock.blockFormat();
			int nIndent = xFmt.indent() - m_nLength; if(nIndent < 0) nIndent = 0;
			xFmt.setIndent(nIndent);
			QTextCursor xTC = pEdit->textCursorOfBlock(xBlock, false);
			xTC.mergeBlockFormat(xFmt);
		}
	};

	qreal lineHeightProportional() const;
	void setLineSpacingProportional(qreal nProportion);

	void removeBlockFromTextList(const QTextBlock& xBlock);

	class _CPredGetTextListStyle
	{
	public:
		QTextListFormat::Style		m_iStyle;
	public:

		_CPredGetTextListStyle() : m_iStyle(QTextListFormat::ListStyleUndefined) {return;}

		void operator()(const _CMyRichEdit* pEdit, const QTextBlock& xBlock, bool& bBreak)
		{
			Q_UNUSED(pEdit);
			QTextList* pList = xBlock.textList();
			if(pList){
				if(m_iStyle == QTextListFormat::ListStyleUndefined){
					m_iStyle = pList->format().style();
				}else{
					if(m_iStyle != pList->format().style()){
						m_iStyle = QTextListFormat::ListStyleUndefined;
						bBreak = true;
					}
				}
			}else{
				m_iStyle = QTextListFormat::ListStyleUndefined;
				bBreak = true;
			}
		}
	};

	template<typename _CPred> void traverseSelectedCells(_CPred& xPred) const
	{
		QTextTable* pTable = QTextEdit::textCursor().currentTable();
		if(pTable){
			bool bBreak = false;
			if(QTextEdit::textCursor().hasComplexSelection()){
				int nRow, nRowCount, nCol, nColCount; QTextEdit::textCursor().selectedTableCells(&nRow, &nRowCount, &nCol, &nColCount);
				for(int x = nCol; x < nCol + nColCount; x++){
					for(int y = nRow; y < nRow + nRowCount; y++){
						QTextTableCell xCell = pTable->cellAt(y, x);
						if(xCell.isValid()){
							xPred(this, pTable, xCell, bBreak);
							if(bBreak) break;
						}
					}
				}
			}else{
				QTextTableCell xCurCell = pTable->cellAt(QTextEdit::textCursor().position());
				xPred(this, pTable, xCurCell, bBreak);
			}
		}
	}

	class _CPredGetMergedCell
	{
	public:
		QList<QTextTableCell>		m_vMergedCells;
	public:

		_CPredGetMergedCell() {return;}

		void operator()(const _CMyRichEdit* pEdit, QTextTable* pTable, const QTextTableCell& xCell, bool& bBreak)
		{
			Q_UNUSED(pEdit); Q_UNUSED(pTable); Q_UNUSED(bBreak);
			if((xCell.rowSpan() > 1) || (xCell.columnSpan() > 1)){
				m_vMergedCells << xCell;
			}
		}
	};

	class _CPredSetCellBackground
	{
	protected:
		QColor		m_clBrush;
	public:

		_CPredSetCellBackground(const QColor& clBrush)
			: m_clBrush(clBrush)
		{return;}

		void operator()(const _CMyRichEdit* pEdit, QTextTable* pTable, const QTextTableCell& xCell, bool& bBreak)
		{
			Q_UNUSED(pTable); Q_UNUSED(bBreak);
			QTextCharFormat xFmt = xCell.format();
			xFmt.setBackground(pEdit->brushOfColor(m_clBrush));
			xCell.firstCursorPosition().mergeBlockCharFormat(xFmt);
		}
	};

	class _CPredSetColWidthConstraints
	{
	public:
		enum Type{
			Increasement = 0
			, FixedValue
		};
	protected:
		Type		m_iType;
		int		m_nValue;

		QList<int>	m_vColHandled;
	public:

		_CPredSetColWidthConstraints(Type iType, int nValue)
			: m_iType(iType)
			, m_nValue(nValue)
		{return;}

		void operator()(const _CMyRichEdit* pEdit, QTextTable* pTable, const QTextTableCell& xCell, bool& bBreak)
		{
			Q_UNUSED(pTable); Q_UNUSED(bBreak);
			int col = xCell.column();
			if(m_vColHandled.indexOf(col) < 0){
				int nValue = -1;
				if(m_iType == FixedValue){
					nValue = m_nValue;
//					pEdit->setColWidthConstraints(col, m_nValue);
				}else if(m_iType == Increasement){
					int width = pEdit->colWidthConstraints(col);
					nValue = width + m_nValue;
//					pEdit->setColWidthConstraints(col, width + m_nValue);
				}

				if(nValue >= 0){
					QTextTableFormat xFmt = pTable->format();
					QVector<QTextLength> vConstraints = xFmt.columnWidthConstraints();
					if(col >= 0 && col < vConstraints.size()){
						vConstraints[col] = QTextLength(QTextLength::FixedLength, nValue >= 0 ? nValue : 0);
					}
					xFmt.setColumnWidthConstraints(vConstraints);
					pTable->setFormat(xFmt);
				}

				m_vColHandled << col;
			}
		}
	};

//	class _CImageUtils{

//	protected:

//		static void selectImages(const QDomElement& xDomEle);

//	public:

//		static QStringList extractImageUrls(const QString& sHtml);

//	};

protected slots:

	void onDetectBlockFmtChanges();
	void onDetectCellsSelectionChanges();
	void onDetectImageSelectionChanged();

	void onModificationChanged(bool bDirty){emit modificationChanged(bDirty);}
	void onContentsChange(int iPosition, int nCharRemoved, int nCharsAdded){emit contentsChange(iPosition, nCharRemoved, nCharsAdded);}

	void onSelectionChanged()
	{
		int nBegin = QTextEdit::textCursor().anchor(), nEnd = QTextEdit::textCursor().position();
		if(nBegin > nEnd) std::swap(nBegin, nEnd);
		emit selectionChanged(nBegin, nEnd);
	}

public:

	_CMyRichEdit(QWidget* parent = 0);

	QString filePath() const {return m_sFilePath;}
	void setFilePath(const QString& sFn){m_sFilePath = sFn;}

	bool isDirty() const {return QTextEdit::document()->isModified();}
	void setDirty(bool bDirty){QTextEdit::document()->setModified(bDirty);}

	int length() const{
//		int iPos = QTextEdit::document()->lastBlock().position();
//		int nLen = QTextEdit::document()->lastBlock().length() - 1;
//		return (iPos + nLen);
		return QTextEdit::document()->characterCount() - 1;
	}

	void open(const QString& sFn);
	void save();
	void saveAs(const QString& sFn) const;

	void setHtmlText(const QString& sHtml, const QUrl& xUrl);

	void beginEdit() const {QTextEdit::textCursor().beginEditBlock();}
	void endEdit() const {QTextEdit::textCursor().endEditBlock();}

	bool hasSelection() const {return QTextEdit::textCursor().hasSelection();}
	void selection(int& nBegin, int& nEnd) const
	{
		nBegin = QTextEdit::textCursor().anchor();
		nEnd = QTextEdit::textCursor().position();
		if(nBegin > nEnd) std::swap(nBegin, nEnd);
	}
	QString selectedText() const {return QTextEdit::textCursor().selectedText();}

	bool isUndoAvailable() const {return QTextEdit::document()->isUndoAvailable();}
	bool isRedoAvailable() const {return QTextEdit::document()->isRedoAvailable();}

	bool fontBold() const;
	void setFontBold(bool b);

	QTextCharFormat::UnderlineStyle fontUnderlineStyle() const;
	void setFontUnderlineStyle(QTextCharFormat::UnderlineStyle iStyle);

	QColor fontUnderlineColor() const;
	void setFontUnderlineColor(const QColor& clUnderline);

	bool fontOverline() const;
	void setFontOverline(bool b);

	bool fontStrikeOut() const;
	void setFontStrikeOut(bool b);

	bool fontSupersript() const;
	void setFontSupersript(bool b);

	bool fontSubsript() const;
	void setFontSubsript(bool b);

	QBrush textForeground() const;
	void setTextForeground(const QBrush& brush);

	QBrush textBackground() const;
	void setTextBackground(const QBrush& brush);

	QTextBlockFormat currentBlockFormat() const;

	Qt::Alignment blockAlignment() const;
	void setBlockAlignment(Qt::Alignment nAlignment);

	int blockIndent() const;
	void setBlockIndent(int nIndent);

	qreal blockTextIndent() const;
	void setBlockTextIndent(qreal nIndent);

	qreal blockLeftMargin() const;
	void setBlockLeftMargin(qreal nMargin) const;

	qreal blockRightMargin() const;
	void setBlockRightMargin(qreal nMargin) const;

	qreal blockTopMargin() const;
	void setBlockTopMargin(qreal nMargin) const;

	qreal blockBottomMargin() const;
	void setBlockBottomMargin(qreal nMargin) const;

	void indent(int nLength)
	{
		_CPredIndentBlocks xPred(nLength);
		beginEdit();
		traverseSelectedBlocks(xPred);
		endEdit();
	}

	void outdent(int nLength)
	{
		_CPredOutdentBlocks xPred(nLength);
		beginEdit();
		traverseSelectedBlocks(xPred);
		endEdit();
	}

	qreal lineHeight() const;
	void setLineSpacing(qreal nSpacing);

	bool isLineSpacingx1() const;
	bool isLineSpacingx15() const;
	bool isLineSpacingx2() const;

	//2016.10.30 ListStyleUndefined means selected blocks(completely or partically) are not list style or in different style
	QTextListFormat::Style blockTextListStyle() const
	{
		_CPredGetTextListStyle xPred;
		traverseSelectedBlocks(xPred);
		return xPred.m_iStyle;
	}

	void setBlockTextList(QTextListFormat::Style iStyle);
	void cancelBlockTextList();

	bool blockOrderedList() const;
	void setBlockOrderedList(bool bList);
	void setBlockOrderedListFmt(const QString& sPrefix, const QString& sSuffix);

	bool blockUnorderedList() const;
	void setBlockUnorderedList(bool bList);

	bool isImageSelected() const {return currentImage();}

	void insertImage(const QPixmap& xImg);

	int imageRotation() const;
	void rotateImage(int nRotation);

	int imageWidth() const;
	int imageHeight() const;
	void scaleImage(qreal x, qreal y);
	void resizeImageTo(int width, int height);

	bool isTableFocused() const {return QTextEdit::textCursor().currentTable();}
	void selectedTableCells(int* rowFirst, int* rowCount, int* colFirst, int* colCount) const;

	void insertTable(int row, int col);
	void insertRows(int row, int num);
	void insertCols(int col, int num);
	void removeRows(int row, int num);
	void removeCols(int col, int num);

	QColor tableBorderColor() const;
	QColor tableBackgroundColor() const;

	QColor cellBackgroundColor(int row, int col) const;
	void setCellBackgroundColor(int row, int col, const QColor& clBrush);

	int colWidthConstraints(int col) const;
	void setColWidthConstraints(int col, int iWidth);

	Qt::Alignment tableAlignment() const;
	void setTableAlignment(Qt::Alignment nAlignment);

//	void setTextFrame(qreal nWidth, QTextFrameFormat::BorderStyle iStyle, const QColor& clColor);
//	void removeTextFrame();

	void insertUrl(const QString& sTitle, const QString& sUrl);
	void setUrlForSelectedText(const QString& sUrl);
	void clearUrlForSelectedText();

public slots:

	void _undo(){m_pTextDocument->_undo();}
	void _redo(){m_pTextDocument->_redo();}

	void toggleFontBold(){setFontBold(!fontBold());}
	void toggleFontItalic(){QTextEdit::setFontItalic(!QTextEdit::fontItalic());}
	void toggleFontUnderline(){QTextEdit::setFontUnderline(!QTextEdit::fontUnderline());}
	void toggleFontOverline(){setFontOverline(!fontOverline());}
	void toggleFontStrikeOut(){setFontStrikeOut(!fontStrikeOut());}
	void toggleFontSuperscript(){setFontSupersript(!fontSupersript());}
	void toggleFontSubscript(){setFontSubsript(!fontSubsript());}

	void setTextForeground(const QColor& color){setTextForeground(brushOfColor(color));}
	void setTextBackground(const QColor& color){setTextBackground(brushOfColor(color));}

	void justifyLeft(){setBlockAlignment(Qt::AlignLeft | Qt::AlignVCenter);}
	void justifyCenter(){setBlockAlignment(Qt::AlignHCenter | Qt::AlignVCenter);}
	void justifyRight(){setBlockAlignment(Qt::AlignRight | Qt::AlignVCenter);}

	void setLineSpacingx1();
	void setLineSpacingx15();
	void setLineSpacingx2();

	void toggleOrderedList(){setBlockOrderedList(!blockOrderedList());}
	void toggleUnorderedList(){setBlockUnorderedList(!blockUnorderedList());}

	void demoteAsChildList();
	void promoteToParentList();

	void indent(){indent(1);}
	void outdent(){outdent(1);}

	void insertRowBefore();
	void insertRowAfter();

	void insertColBefore();
	void insertColAfter();

	void removeRows();
	void removeCols();

	void mergeTableCells();
	void splitTableCells();

	void setTableBorderColor(const QColor& pen);
	void setTableBackgroundColor(const QColor& brush);
	void setCellsBackgroundColor(const QColor& brush);

	void setColsWidthConstraints(int width);
	void increaseColsWidthConstraints(int increasement);

	void highlightOccurrences(const QString& sText, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp);
	bool findNext(const QString& sText, bool bWrapsAround, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp, bool bBackward);
	bool replaceText(const QString& sText, const QString& sReplaceWith, bool bWrapsAround, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp);
	int replaceAll(const QString& sText, const QString& sReplaceWith, bool bCaseSensitive, bool bMatchWholeWord, bool bUseRegExp);

signals:

	void linkHovered(const QString& sUrl);
	void linkClicked(const QString& sUrl);

	void modificationChanged(bool bDirty);
	void contentsChange(int iPosition, int nCharRemoved, int nCharsAdded);

	void currentBlockFormatChanged(const QTextBlockFormat& xBlockFmt, QTextListFormat::Style iStyle);

	void selectionChanged(int nBegin, int nEnd);
	void imageSelectionChanged(bool bSelected);
	//2016.11.26 if table is not focused, the nRowFirst and the nColFirst will be both -1
	void cellsSelectionChanged(int nRowFirst, int nRowCount, int nColFirst, int nColCount);

};

#endif // _CMYRICHEDIT_H
