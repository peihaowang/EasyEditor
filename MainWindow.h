#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextCharFormat>
#include <QMainWindow>

class QAction;
class QActionColor;
class QActionGroup;
class QToolButton;
class QToolButtonColor;
class QMenuColor;
class QMenuUnderline;
class QComboBox;
class QFontComboBox;
class _CMyRichEdit;
class QLineEdit;
class QTabWidget;
class QLabel;
class QPanelFindReplace;

class MainWindow : public QMainWindow
{
	Q_OBJECT

protected:

	QTabWidget *			m_pTabDocuments;
	_CMyRichEdit *			m_pCurrentEdit;
	QPanelFindReplace *		m_pPanelFindReplace;

	QAction *			m_pActionNew;
	QAction *			m_pActionOpen;
	QAction *			m_pActionSave;

	QAction *			m_pActionExportAsHtml;
	QAction *			m_pActionExportAsPdf;
	QAction *			m_pActionExportAsText;

	QAction *			m_pActionUndo;
	QAction *			m_pActionRedo;

	QAction *			m_pActionCut;
	QAction *			m_pActionCopy;
	QAction *			m_pActionPaste;

	QAction *			m_pActionFindText;
	QAction *			m_pActionReplaceText;

	QAction *			m_pActionEditHyperlink;
	QAction *			m_pActionCancelHyperlink;

	bool				m_bTextFamilySetable;
	QFontComboBox *			m_pComboTextFamily;

	bool				m_bTextSizeSetable;
	QComboBox *			m_pComboTextSize;

	QAction *			m_pActionFont;
	QAction *			m_pActionBold;
	QAction *			m_pActionItalic;
	QAction *			m_pActionUnderline;
	QMenuUnderline *		m_pMenuUnderline;
	QAction *			m_pActionOverline;
	QAction *			m_pActionStrikeOut;
	QAction *			m_pActionSubscript;
	QAction *			m_pActionSuperscript;

	QActionColor *			m_pActionTextForeColor;
	QActionColor *			m_pActionTextBackColor;
	QToolButtonColor *		m_pBtnTextForeColor;
	QToolButtonColor *		m_pBtnTextBackColor;

	QAction *			m_pActionJustifyLeft;
	QAction *			m_pActionJustifyCenter;
	QAction *			m_pActionJustifyRight;

	QAction *			m_pActionParaMargin;
	QAction *			m_pActionIndent;
	QAction *			m_pActionOutdent;

	QAction *			m_pActionLineSpacingx1;
	QAction *			m_pActionLineSpacingx15;
	QAction *			m_pActionLineSpacingx2;

	QAction *			m_pActionInsOrderedList;
	QAction *			m_pActionInsUnorderedList;
	QActionGroup *			m_pActionGrpBlockList;

	QAction *			m_pActionInsImage;
	QAction *			m_pActionRotateImage;
	QAction *			m_pActionScaleImage;
	QAction *			m_pActionExportImage;

	QAction *			m_pActionInsTable;
	QAction *			m_pActionInsRowBefore;
	QAction *			m_pActionInsRowAfter;
	QAction *			m_pActionInsColBefore;
	QAction *			m_pActionInsColAfter;
	QAction *			m_pActionRemoveRows;
	QAction *			m_pActionRemoveCols;
	QAction *			m_pActionMergeCells;
	QAction *			m_pActionSplitCells;
	QActionColor *			m_pActionTableBorderColor;
	QActionColor *			m_pActionCellBackColor;
#if !defined(Q_OS_MAC)
	QToolButtonColor *		m_pBtnTableBorderColor;
	QToolButtonColor *		m_pBtnCellBackColor;
#endif
	QAction *			m_pActionWidenCols;
	QAction *			m_pActionNarrowCols;

	QAction *			m_pActionGitHub;
	QAction *			m_pActionAboutQt;
	QAction *			m_pActionAbout;

	QLabel *			m_pDisplayDefault;
	QLabel *			m_pDisplayExtraInfo;

public:

	MainWindow(QWidget* parent = 0);
	~MainWindow();

protected:

	virtual void keyPressEvent(QKeyEvent* e);
	virtual void closeEvent(QCloseEvent* e);

	void makeUniqueConnection(const QObject* sender, const char* signal, const QObject* prevReceiver, const QObject* newReceiver, const char* member);
	void makeUniqueConnection(const QObject* prevSender, const QObject* newSender, const char* signal, const QObject* receiver, const char* member);
	void makeConnections(_CMyRichEdit* pPrevEdit, _CMyRichEdit* pNewEdit);

	void createEdit(const QString& sFilePath);

	void updateEditActionsState();

	QString titleOfDocument(int nIndex);
	void updateWindowTitle();
	void updateTabTitle(int nIndex);

	bool saveFile(int nIndex);

	bool confirmSaveFile(int nIndex);
//	bool confirmSaveFiles();

protected slots:

	void onSwitchTab(int index);
	void onTabCloseRequested(int index);

	void onUpdateEditorInfo();

	void onLoadRecentFiles();

	void onNewFile();
	void onOpenFile();
	void onSaveFile();

	void onExportAsHtml();
	void onExportAsPdf();
	void onExportAsText();

	void onFindText();
	void onReplaceText();
	void onFindPanelVisibilityChanged(bool bVisible);

	void onCustomFont();

	void onUnderline();
	void onSetUnderlineColor(const QColor& clColor);
	void onSetUnderlineStyle(QTextCharFormat::UnderlineStyle iStyle);

	void onTextFamily(const QFont& ftFont);
//	void onTextSize(int nIndex);
	void onTextSize(const QString& sText);

	void onParaMargin();

	void onSetListStyle();

	void onInsertImage();
	void onExportImage();
	void onRotateImage();
	void onScaleImage();

	void onInsertTable();
	void onWidenSelCols();
	void onNarrowSelCols();

	void onEditHyperlink();
	void onCancelHyperlink();

	void onViewGitHub();
	void onAbout();

	void onHyperlinkHovered(const QString& sUrl);
	void onHyperlinkClicked(const QString& sUrl);

	void onUndoAvailable(bool bCan);
	void onRedoAvailable(bool bCan);
	void onModificationChanged(bool bModified);

	void onCurrentCharFormatChanged(const QTextCharFormat& xFmt);
	void onCurrentBlockFormatChanged(const QTextBlockFormat& xFmt, QTextListFormat::Style iStyle);
	void onTextSelectionChanged(int nBegin, int nEnd);
	void onImageSelectionChanged(bool bSelected);
	void onCellsSelectionChanged(int nRowFirst, int nRowCount, int nColFirst, int nColCount);

	void onContextMenuRequest(const QPoint& pos);

};

#endif // MAINWINDOW_H
