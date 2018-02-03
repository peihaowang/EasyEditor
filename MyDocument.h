#ifndef _CMYDOCUMENT_H
#define _CMYDOCUMENT_H

#include <QtDebug>
#include <QUrl>
#include <QTextDocument>
#include <QDomDocument>
#include <QUndoCommand>
#include <QSyntaxHighlighter>

class _CMyRichEdit;

class _CHighlighterOccurrence : public QSyntaxHighlighter
{

private:

	QString				m_sText2Find;
	bool				m_bMatchCase;
	bool				m_bMatchWholeWords;
	bool				m_bUseRegExp;
	QTextCharFormat			m_xFmtOccurrence;

public:

	_CHighlighterOccurrence(QTextDocument* parent = 0);

	QString text2Find() const {return m_sText2Find;}
	void setText2Find(const QString& sText){m_sText2Find = sText;}

	bool caseSensitive() const {return m_bMatchCase;}
	void setCaseSensitivity(bool bCase){m_bMatchCase = bCase;}

	bool matchWholeWords() const {return m_bMatchWholeWords;}
	void setMatchWholeWords(bool bWholeWord){m_bMatchWholeWords = bWholeWord;}

	bool useRegExp() const {return m_bUseRegExp;}
	void setUseRegExp(bool bRegExp){m_bUseRegExp = bRegExp;}

	QColor highlightColor() const;
	void setHighlightColor(const QColor& clColor);

protected:

	virtual void highlightBlock(const QString& sText);

};

class _CTextImage{

protected:

	QString			m_sUrl;

	qreal			m_nScaleX;
	qreal			m_nScaleY;
	int			m_nRotation;

	QString			m_sSource;
	QString			m_sFormat;

protected:

	void _clear()
	{
		m_sUrl = "";

		m_nScaleX = 1.0;
		m_nScaleY = 1.0;
		m_nRotation = 0;

		m_sSource = "";
		m_sFormat = "";
	}

	void _unserialize(const QDomElement& xDomEle);
	void _serialize(QDomElement& xDomEle) const;

public:

	_CTextImage(){_clear();}

	_CTextImage(const QDomElement& xDomEle){_clear(); _unserialize(xDomEle);}
	_CTextImage(const QPixmap& xImage, const QString& sFormat = ""){_clear(); setSource(xImage, sFormat);}
	_CTextImage(const _CTextImage& xTextImage){(*this) = xTextImage;}

	bool isValid() const {return !m_sSource.isEmpty();}

	QString format() const {return m_sFormat;}

	qreal scaleValX() const {return m_nScaleX;}
	qreal scaleValY() const {return m_nScaleY;}
	void scale(qreal nScaleX, qreal nScaleY){m_nScaleX = nScaleX; m_nScaleY = nScaleY;}

	int rotation() const {return m_nRotation;}
	void rotate(int nRotation){m_nRotation = (nRotation % 360);}

	QString url() const {return m_sUrl;}
	void setUrl(const QString& sUrl){m_sUrl = sUrl;}

	QString source() const {return m_sSource;}
	QPixmap sourcePixmap() const;
	QPixmap transformedPixmap(Qt::TransformationMode iTransformMode = Qt::FastTransformation) const;
	void setSource(const QPixmap& xPixmap, const QString& sFormat = "");

	QString imageTagStr() const {return QString("<img src=\"%1\" />").arg(m_sUrl);}

	_CTextImage& operator=(const _CTextImage& xOther)
	{
		m_nScaleX = xOther.m_nScaleX;
		m_nScaleY = xOther.m_nScaleY;
		m_nRotation = xOther.m_nRotation;

		m_sUrl = xOther.m_sUrl;
		m_sSource = xOther.m_sSource;
		m_sFormat = xOther.m_sFormat;

		return (*this);
	}
	void operator=(const QDomElement& xDomEle){_unserialize(xDomEle);}
	void constructDomElement(QDomElement& xDomEle) const {_serialize(xDomEle);}
	QTextImageFormat constructTextImageFmt() const;

};

class _CMyDocument;

class _CUndoCmdGeneral : public QUndoCommand{

protected:

	_CMyDocument *			m_pTextDocument;
	bool				m_bVirgin;

public:

	_CUndoCmdGeneral(_CMyDocument* pDocument, QUndoCommand* parent = 0);

	virtual void undo();
	virtual void redo();

};

class _CUndoCmdInsertImage : public QUndoCommand{

protected:

	_CMyDocument *			m_pTextDocument;
	_CTextImage			m_xTextImage;

public:

	_CUndoCmdInsertImage(_CMyDocument* pDocument, const _CTextImage& xTextImage, QUndoCommand* parent = 0);

	virtual void undo();
	virtual void redo();

};

class _CUndoCmdRotateImage : public QUndoCommand{

protected:

	_CMyDocument *			m_pTextDocument;

	QString				m_sUrl;
	int				m_nVal;

public:

	_CUndoCmdRotateImage(_CMyDocument* pDocument, const QString& sUrl, int nVal, QUndoCommand* parent = 0);

	void swap();

	virtual void undo(){swap();}
	virtual void redo(){swap();}

};

class _CUndoCmdScaleImage : public QUndoCommand{

protected:

	_CMyDocument *			m_pTextDocument;

	QString				m_sUrl;
	double				m_nValX;
	double				m_nValY;

public:

	_CUndoCmdScaleImage(_CMyDocument* pDocument, const QString& sUrl, double nValX, double nValY, QUndoCommand* parent = 0);

	void swap();

	virtual void undo(){swap();}
	virtual void redo(){swap();}

};

class _CMyDocument : public QTextDocument{

	Q_OBJECT

protected:

	QMap<QUrl, _CTextImage>		m_mImageResources;
	QMap<QUrl, QPixmap>		m_mImageCache;

	QUndoStack *			m_pUndoStack;

protected:

	class _CPredCollectImgUrls
	{
	public:
		QList<QUrl>		m_vImgUrls;
	public:
		void operator()(QDomElement& xDomEle);
	};

	class _CPredRestoreImgSource
	{
	public:
		const _CMyDocument *	m_pTextDocument;
	public:
		_CPredRestoreImgSource(const _CMyDocument* pTextDocument) : m_pTextDocument(pTextDocument){return;}
		void operator()(QDomElement& xDomEle);
	};

public:

	friend class _CMyRichEdit;

	_CMyDocument(QObject* parent);

	void loadDocument(const QString& sText);
	QString content() const;
	QString convertToHtml() const;

	bool existsImage(const QUrl& xUrlImg) const;
	//2018.1.30 It's not recommend to keep the returned pointer for a long period
	const _CTextImage* imageOf(const QUrl& xUrlImg) const;
	_CTextImage* imageOf(const QUrl& xUrlImg);

	void addImageResource(const QUrl& xUrlImg, const _CTextImage& xTextImage){m_mImageResources[xUrlImg] = xTextImage;}
	void removeImageResource(const QUrl& xUrlImg){m_mImageResources.remove(xUrlImg);}
	void cleanUnlinkedImages();

	void clearImageCache(const QUrl& xUrlImg = QUrl());

	void beginMacro(const QString& sText);
	void endMacro();
	void pushToUndoStack(QUndoCommand* pUndoCmd);

	void _undo();
	void _redo();

protected slots:

	void onUndoCommandAdded();

protected:

	QVariant loadResource(int type, const QUrl& name);

};

#endif // _CMYDOCUMENT_H
