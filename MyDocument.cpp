#include "common_headers.h"
#include "MyRichEdit.h"
#include "MyDocument.h"

_CHighlighterOccurrence::_CHighlighterOccurrence(QTextDocument* parent)
	: QSyntaxHighlighter(parent)
	, m_bMatchCase(false)
	, m_bMatchWholeWords(false)
{
//	m_xFmtOccurrence.setBackground(QBrush(QColor(255, 150, 50)));
	m_xFmtOccurrence.setBackground(QBrush(QColor(255, 239, 11)));
}

QColor _CHighlighterOccurrence::highlightColor() const
{
	return m_xFmtOccurrence.background().color();
}

void _CHighlighterOccurrence::setHighlightColor(const QColor& clColor)
{
	m_xFmtOccurrence.setBackground(QBrush(clColor));
}

void _CHighlighterOccurrence::highlightBlock(const QString& sText)
{
	if(!m_sText2Find.isEmpty()){
		QString sTxt2Find = m_bUseRegExp ? m_sText2Find : QRegExp::escape(m_sText2Find);
		Qt::CaseSensitivity iCase = m_bMatchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
		QString sPattern = m_bMatchWholeWords ? QString("\\b%1\\b").arg(sTxt2Find) : sTxt2Find;
		QRegExp xExp(sPattern, iCase);
		int nIndex = sText.indexOf(xExp, 0);
		while(nIndex >= 0){
			int nLength = xExp.matchedLength();
			if(nLength <= 0) break;

			QSyntaxHighlighter::setFormat(nIndex, nLength, m_xFmtOccurrence);
			nIndex = sText.indexOf(xExp, nIndex + nLength);
		}
	}
}

///////////////////////////////////////////////////////////

_CBlockBlink::_CBlockBlink(_CMyRichEdit* edit)
	: QObject(edit)
	, m_pEdit(edit)
{
	m_pTimer = new QTimer(this);
	m_pTimer->setInterval(50);
	QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onFadeIn()));
}

void _CBlockBlink::blink(const QList<QTextBlock>& vBlocks)
{
	if(m_pEdit){
		m_pTimer->stop();
		m_clBackground = QColor(70, 162, 218, 255);
		QList<QTextEdit::ExtraSelection> vSelections;
		Q_FOREACH(QTextBlock xBlock, vBlocks){
			QTextEdit::ExtraSelection xSelection;
			QTextCharFormat xFmt;
			xFmt.setBackground(m_clBackground);
			xFmt.setProperty(QTextFormat::FullWidthSelection, true);
			xSelection.cursor = m_pEdit->textCursorOfBlock(xBlock, true);
			xSelection.format = xFmt;
			vSelections << xSelection;
		}
		m_pEdit->setExtraSelections(vSelections);
		m_pTimer->start();
	}
}

void _CBlockBlink::onFadeIn()
{
	if(m_pEdit){
		int alpha = m_clBackground.alpha(); alpha -= 12;
		if(alpha <= 10){
			m_pTimer->stop();
			m_pEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>());
		}else{
			m_clBackground.setAlpha(alpha);
			QList<QTextEdit::ExtraSelection> vNewSelections, vSelections = m_pEdit->extraSelections();
			Q_FOREACH(QTextEdit::ExtraSelection xSelection, vSelections){
				QTextEdit::ExtraSelection xNewSelection = xSelection;
				xNewSelection.format.setBackground(m_clBackground);
				vNewSelections << xNewSelection;
			}
			m_pEdit->setExtraSelections(vNewSelections);
		}
	}
}

///////////////////////////////////////////////////////////

void _CTextImage::_unserialize(const QDomElement& xDomEle)
{
	if(xDomEle.tagName() == "imgres"){
		if(xDomEle.hasAttribute("scaleX")) m_nScaleX = xDomEle.attribute("scaleX").toDouble();
		if(xDomEle.hasAttribute("scaleY")) m_nScaleY = xDomEle.attribute("scaleY").toDouble();
		if(xDomEle.hasAttribute("rotation")) m_nRotation = xDomEle.attribute("rotation").toInt();

		if(xDomEle.hasAttribute("url")) m_sUrl = xDomEle.attribute("url");
		if(xDomEle.hasAttribute("source")) m_sSource = xDomEle.attribute("source");
		if(xDomEle.hasAttribute("format")) m_sFormat = xDomEle.attribute("format");
	}
}

void _CTextImage::_serialize(QDomElement& xDomEle) const
{
	if(xDomEle.tagName() == "imgres"){
		xDomEle.setAttribute("scaleX", m_nScaleX);
		xDomEle.setAttribute("scaleY", m_nScaleY);
		xDomEle.setAttribute("rotation", m_nRotation);
		xDomEle.setAttribute("url", m_sUrl);
		xDomEle.setAttribute("source", m_sSource);
		xDomEle.setAttribute("format", m_sFormat);
	}
}

QPixmap _CTextImage::sourcePixmap() const
{
	QPixmap xImg;
	if(!m_sSource.isEmpty()){
		QByteArray vData; vData = QByteArray::fromBase64(m_sSource.toUtf8());
		xImg.loadFromData(vData, m_sFormat.isEmpty() ? NULL : m_sFormat.toLatin1().data());
	}
	return xImg;
}

void _CTextImage::setSource(const QPixmap& xPixmap, const QString& sFormat)
{
	m_sFormat = sFormat; if(m_sFormat.isEmpty()) m_sFormat = "png";
	m_sSource = _CPixmapEx(xPixmap).toBase64(m_sFormat);

	{
		QByteArray vKey;
		{
			QBuffer vBuf(&vKey);
			xPixmap.save(&vBuf, m_sFormat.toLatin1().data());
		}

		{
			QDataStream xStrm(&vKey, QIODevice::Append); xStrm.setVersion(QDataStream::Qt_5_5);
			xStrm << QDateTime::currentDateTime().toMSecsSinceEpoch();
		}

		{
			QString sName = _CLocalFile::changeExtension(QString::number(qHash(vKey)), m_sFormat);
			QUrl xUrl;
			xUrl.setScheme("embeddedimg");
			xUrl.setPath(sName, QUrl::DecodedMode);
			m_sUrl = xUrl.toString();
		}
	}
}

///////////////////////////////////////////////////////////

_CUndoCmdGeneral::_CUndoCmdGeneral(_CMyDocument* pDocument, QUndoCommand* parent)
	: QUndoCommand(parent)
	, m_pTextDocument(pDocument)
	, m_bVirgin(true)
{
	return;
}

void _CUndoCmdGeneral::undo()
{
	m_pTextDocument->undo();
}

void _CUndoCmdGeneral::redo()
{
	if(!m_bVirgin){
		m_pTextDocument->redo();
	}else{
		m_bVirgin = false;
	}
}

///////////////////////////////////////////////////////////

_CUndoCmdInsertImage::_CUndoCmdInsertImage(_CMyDocument* pDocument, const _CTextImage& xTextImage, QUndoCommand* parent)
	: QUndoCommand(parent)
	, m_pTextDocument(pDocument)
	, m_xTextImage(xTextImage)
{
	return;
}

void _CUndoCmdInsertImage::undo(){
	m_pTextDocument->m_mImageResources.remove(QUrl(m_xTextImage.url()));
	m_pTextDocument->m_mImageCache.remove(m_xTextImage.url());
}

void _CUndoCmdInsertImage::redo(){
	m_pTextDocument->m_mImageResources[QUrl(m_xTextImage.url())] = m_xTextImage;
}

///////////////////////////////////////////////////////////

_CUndoCmdRotateImage::_CUndoCmdRotateImage(_CMyDocument* pDocument, const QString& sUrl, int nVal, QUndoCommand* parent)
	: QUndoCommand(parent)
	, m_pTextDocument(pDocument)
	, m_sUrl(sUrl)
	, m_nVal(nVal)
{
	return;
}

void _CUndoCmdRotateImage::swap()
{
	QMap<QUrl, _CTextImage>::iterator it = m_pTextDocument->m_mImageResources.find(QUrl(m_sUrl));
	if(it != m_pTextDocument->m_mImageResources.end()){
		int nOldVal = it.value().rotation();
		it.value().rotate(m_nVal);
		m_nVal = nOldVal;

		m_pTextDocument->m_mImageCache.remove(QUrl(m_sUrl));
	}
}

///////////////////////////////////////////////////////////

_CMyDocument::_CMyDocument(QObject* parent)
	: QTextDocument(parent)
{
	m_pUndoStack = new QUndoStack(this);
	QObject::connect(this, SIGNAL(undoCommandAdded()), this, SLOT(onUndoCommandAdded()));
}

void _CMyDocument::loadDocument(const QString& sText, QString* sErrMsg, int* nErrLine, int* nErrCol)
{
	QDomDocument xDomDoc; xDomDoc.setContent(sText, sErrMsg, nErrLine, nErrCol);

//	QRegularExpression xRE("^(\\s*)\\n(\\s*)\\n", QRegularExpression::MultilineOption);
//	QRegularExpressionMatch xMatch; sText.indexOf(xRE, 0, &xMatch);

//	if(xMatch.isValid()){
//		QString sResources = sText.mid(0, xMatch.capturedStart() + 1);
//		QString sHtml = sText.mid(xMatch.capturedEnd() + 1);

//		//2018.1.29 Identify resources
//		{
//			QDomDocument xDomDocRes; xDomDocRes.setContent(sResources);
//			if(!xDomDocRes.isNull()){
//				QDomNodeList vImgs = xDomDocRes.documentElement().elementsByTagName("imgres");
//				for(int i = 0; i < vImgs.count(); i++){
//					QDomNode xNode = vImgs.item(i);
//					if(xNode.isElement()){
//						_CTextImage xTextImg(xNode.toElement());
//						m_mImageResources[QUrl(xTextImg.url())] = xTextImg;
//					}
//				}
//			}
//		}

//		//2018.1.23 Set html content
//		QTextDocument::setHtml(sHtml);
//		QTextDocument::setModified(false);
//	}


	//2018.1.23 Identify resources
	{
		QDomElement xEleHead = xDomDoc.documentElement().firstChildElement("head");
		if(!xEleHead.isNull()){
			QDomElement xEleResources = xEleHead.firstChildElement("__resources__");
			if(!xEleResources.isNull()){
				QDomNodeList vImgs = xEleResources.elementsByTagName("imgres");
				for(int i = 0; i < vImgs.count(); i++){
					QDomNode xNode = vImgs.item(i);
					if(xNode.isElement()){
						_CTextImage xTextImg(xNode.toElement());
						m_mImageResources[QUrl(xTextImg.url())] = xTextImg;
					}
				}
			}
//			xEleHead.removeChild(xEleResources);
		}
	}

	//2018.1.23 Set html content
//	QTextDocument::setHtml(xDomDoc.toString());
	QTextDocument::setHtml(sText);
	QTextDocument::setModified(false);
}

QString _CMyDocument::toString(QString* sErrMsg, int* nErrLine, int* nErrCol) const
{
	QDomDocument xDomDoc; xDomDoc.setContent(QTextDocument::toHtml(), sErrMsg, nErrLine, nErrCol);
	QDomElement xEleHead = xDomDoc.documentElement().firstChildElement("head");
	if(xEleHead.isNull()){
		xEleHead = xDomDoc.createElement("head");
		xDomDoc.documentElement().appendChild(xEleHead);
	}
	QDomElement xEleResources = xDomDoc.createElement("__resources__");
	if(!xEleResources.isNull()){
		QMap<QUrl, _CTextImage>::const_iterator it;
		for(it = m_mImageResources.begin(); it != m_mImageResources.end(); it++){
			QDomElement xEleImg = xDomDoc.createElement("imgres");
			it.value().constructDomElement(xEleImg);
			xEleResources.appendChild(xEleImg);
		}
		xEleHead.appendChild(xEleResources);
	}
	return xDomDoc.toString(-1);
}

QPixmap _CMyDocument::sourceImage(const _CTextImage& xTextImg, bool bOrigin) const
{
	QPixmap xSource = xTextImg.sourcePixmap();
	if(!xSource.isNull() && !bOrigin){
		qreal nScaleX = xTextImg.scaleValX(), nScaleY = xTextImg.scaleValY();
		int rotation = xTextImg.rotation() > 0 ? xTextImg.rotation() : 0;

		Qt::TransformationMode nMode = Qt::FastTransformation; // Qt::SmoothTransformation
		QTransform xTransform;
		if(rotation != 0){
			xTransform.translate((double)xSource.width() / 2.0, (double)xSource.height() / 2.0);	//2016.10.23 Set rotation center
			xTransform.rotate(rotation);
		}
		if(nScaleX != 1.0 || nScaleY != 1.0) xTransform.scale(nScaleX, nScaleY);
		xSource = xSource.transformed(xTransform, nMode);
	}

	return xSource;
}

void _CMyDocument::pushToUndoStack(QUndoCommand* pUndoCmd)
{
	m_pUndoStack->push(pUndoCmd);
}

void _CMyDocument::beginMacro(const QString& sText)
{
	m_pUndoStack->beginMacro(sText);
}

void _CMyDocument::endMacro()
{
	m_pUndoStack->endMacro();
}

void _CMyDocument::_undo()
{
	if(m_pUndoStack->canUndo()){
		m_pUndoStack->undo();
	}
}

void _CMyDocument::_redo()
{
	if(m_pUndoStack->canRedo()){
		m_pUndoStack->redo();
	}
}

void _CMyDocument::cleanUnlinkedImages()
{
	QDomDocument xDomDoc; xDomDoc.setContent(QTextDocument::toHtml());
	QDomElement xEleBody = xDomDoc.documentElement().firstChildElement("body");
	if(!xEleBody.isNull()){
		_CPredCollectImgUrls xPred;
		_CXmlUtils::travelDomChildElements(xEleBody, xPred);

		QList<QUrl> vToRemove;
		QMap<QUrl, _CTextImage>::iterator it;
		for(it = m_mImageResources.begin(); it != m_mImageResources.end(); it++){
			if(xPred.m_vImgUrls.indexOf(it.key()) < 0){
				vToRemove << it.key();
			}
		}
		Q_FOREACH(QUrl xUrl, vToRemove){
			m_mImageResources.remove(xUrl);
		}
	}
}

QVariant _CMyDocument::loadResource(int type, const QUrl& name)
{
	QVariant xRes;
	if(type == QTextDocument::ImageResource){
		if(name.scheme() == "embeddedimg"){
			QPixmap xImg;
			QMap<QUrl, QPixmap>::const_iterator itCache = m_mImageCache.find(name);
			if(itCache != m_mImageCache.end()){
				xImg = itCache.value();
			}else{
				QMap<QUrl, _CTextImage>::const_iterator itRes = m_mImageResources.find(name);
				if(itRes != m_mImageResources.end()){
					xImg = sourceImage(itRes.value(), false);
					m_mImageCache[name] = xImg;
				}
			}
			if(!xImg.isNull()) xRes = QVariant(xImg);
		}
	}
	if(xRes.isNull()){
		xRes = QTextDocument::loadResource(type, name);
	}
	return xRes;
}


void _CMyDocument::onUndoCommandAdded()
{
	m_pUndoStack->push(new _CUndoCmdGeneral(this));
}
