#ifndef MISC_UTILITIES_H
#define MISC_UTILITIES_H

#include <ctime>
#include <math.h>

#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QFileIconProvider>
#include <QDateTime>
#include <QFontDialog>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QLayout>
#include <QLayoutItem>
#include <QApplication>

#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomElement>

#include <QSvgRenderer>

#include "type_defs.h"
#include "string_utilities.h"
#include "cpp_utilities.h"

///////////////////////////////////////////////////////////

#define	_CHAR_SLASH		'/'
#define	_CHAR_BACKSLASH		'\\'
#define	_CHAR_DOT		'.'

#define	_TEXT_SLASH		QString(_CHAR_SLASH)
#define	_TEXT_BACKSLASH		QString(_CHAR_BACKSLASH)
#define	_TEXT_DOT		QString(_CHAR_DOT)

///////////////////////////////////////////////////////////

#define		__ASSERT(x)	//Q_ASSERT(x)

///////////////////////////////////////////////////////////

template<typename T> class _CPredQVector : public QVector<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredQList : public QList<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredQSet : public QSet<T>{

public:

	virtual void operator()(const T& t){this->insert(t);}

};

///////////////////////////////////////////////////////////

class _CTemporaryFile{

private:

	QString		 m_sTmpDir;

public:

	_CTemporaryFile(void) : m_sTmpDir(QDir::tempPath()){return;}
	_CTemporaryFile(const QString& sTmpDir)
		: m_sTmpDir(sTmpDir)
	{
		return;
	}

	QString _getTempFile(const QString& sPrefix0="", const QString& sSuffix0="")
	{
		//2014.9.13 allows to customize the prefix as well;
		QString sTmpFn, sPre=sPrefix0, sSuf=sSuffix0; sSuf.replace(QString("."), QString(""));
		if(sPre.isEmpty()) sPre="~ph";
		if(sSuf.isEmpty()) sSuf="tmp";
		do{
			QString sName=QString("%1%2_%3.%4")
					.arg(sPre)
					.arg(time(NULL), 0, 16)
					.arg(qrand(), 0, 16)
					.arg(sSuf)
					;
			QFileInfo f(m_sTmpDir, sName);
			if(!f.exists()){
				sTmpFn=f.filePath();
				break;
			}
		}while(true);
		return sTmpFn;
	}

//	QString _getTempDir(const QString& sInfo = "")
//	{
//		QString sTmpFn;
//		do{
//			QString sName = QString("%1-%2-%3")
//					.arg(sInfo)
//					.arg(time(NULL), 0, 16)
//					.arg(qrand(), 0, 16)
//					;
//			QFileInfo f(m_sTmpDir, sName);
//			if(!f.exists()){
//				sTmpFn=f.filePath();
//				break;
//			}
//		}while(true);
//		return sTmpFn;
//	}

};

///////////////////////////////////////////////////////////

class _CDeferFileDeletion{

private:

	QString				m_sFnToDel;
	bool				m_bTrackErr; //2014.4.9 whether to keep the file name in buffer if failed to delete it;

	static QStringList		s_vFailed; //2014.4.9 files that have problems being deleted;

public:

	~_CDeferFileDeletion(void){perform();}
	_CDeferFileDeletion(const QString& sFn, bool bTrackErr=true)
		: m_sFnToDel(sFn)
		, m_bTrackErr(bTrackErr)
	{
		return;
	}

	bool perform(void)
	{
		QFile::remove(m_sFnToDel);
		bool bExist=QFileInfo(m_sFnToDel).exists();
		if(bExist && m_bTrackErr){
			s_vFailed << m_sFnToDel;
		}
		return bExist;
	}

public:

	static QStringList& getFailed(void){return s_vFailed;}
	static void clearFailed(void){s_vFailed.clear();}

};

///////////////////////////////////////////////////////////

class _CLocalFile{

public:

	static QString invalidChars(){return QString("\\/:*?\"<>|");}

	static bool isFileNameValid(const QString& sName)
	{
		bool bRes = true;
		if(!sName.isEmpty()){
			QString sBad = invalidChars();
			for(int sub = 0; sub < sBad.size(); sub++){
				QChar ch = sBad[sub];
				if(sName.indexOf(ch) >= 0){
					bRes = false;
					break;
				}
			}
		}else{
			bRes = false;
		}
		return bRes;
	}

	static QString eliminateBeginingSlashes(const QString& sPath0)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=0; i<nLen; ++i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash>0) sPath = sPath.right(sPath.size()-nSlash);
		return sPath;
	}

	static QString eliminateTrailingSlashes(const QString& sPath0)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=nLen-1; i>=0; --i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash>0) sPath = sPath.left(sPath.size()-nSlash);
		return sPath;
	}

	static QString ensureBeginingSlashes(const QString& sPath0, QChar chSlash = _CHAR_SLASH)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=0; i<nLen; ++i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash==0) sPath = chSlash + sPath;
		else if(nSlash>1) sPath = sPath.right(nSlash-1); //keep only one slash begining;
		return sPath;
	}

	static QString ensureTrailingSlashes(const QString& sPath0, QChar chSlash = _CHAR_SLASH)
	{
		QString sPath = sPath0;
		#if 0
		if(!sPath.isEmpty()){
			sPath = eliminateTrailingSlashes(sPath0);
			sPath += chSlash;
		}
		#else
		int nSlash = 0, nLen = sPath.size();
		for(int i=nLen-1; i>=0; --i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash==0) sPath += chSlash;
		else if(nSlash>1) sPath = sPath.left(sPath.size()-nSlash+1); //keep only one slash trailing;
		#endif
		return sPath;
	}

	static QString concatenate(const QString& sLeft0, const QString& sRight0, QChar chSlash)
	{return concatenate(sLeft0, sRight0, QString(chSlash));}

	static QString concatenate(const QString& sLeft0, const QString& sRight0, const QString& sSlash = _TEXT_SLASH)
	{
		QString sLeft = eliminateTrailingSlashes(sLeft0);
		QString sRight = eliminateBeginingSlashes(sRight0);
		return sLeft + sSlash + sRight;
	}

	static QString dirOf(const QString& sPath, bool bSlash)
	{
		//2014.2.26 ensure a trailing slash if result is not empty;
		QString sDir, sTmp = sPath; sTmp.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
		if(!sTmp.isEmpty()){
			int p=sTmp.lastIndexOf(_CHAR_SLASH);
			if(p>=0){
				if(bSlash) p++;
				sDir=sTmp.left(p);
			}
		}
		return sDir;
	}

	static QString parentOf(const QString& sPath)
	{
		//2014.2.26 ensure a trailing slash if result is not empty;
		QString sDir, sTmp = sPath; sTmp.replace(_CHAR_BACKSLASH, _CHAR_SLASH);

		//be sure to first get rid of the trialing slash;
		while(!sTmp.isEmpty() && sTmp.at(sTmp.size()-1)==_CHAR_SLASH){
			sTmp=sTmp.left(sTmp.size()-1);
		}

		if(!sTmp.isEmpty()){
			int p=sTmp.lastIndexOf(_CHAR_SLASH);
			if(p>=0){
				sDir=sTmp.left(p); //excluding the trailing slash;
			}
		}
		return sDir;
	}

	static QString leafOf(const QString& sPath)
	{
		QString sLeaf = sPath; sLeaf.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
		if(!sLeaf.isEmpty()){
			int p = sLeaf.lastIndexOf(_CHAR_SLASH);
			if(p >= 0){
				sLeaf = sLeaf.right(sLeaf.size()-p-1);
			}
		}
		return sLeaf;
	}

	static QString titleOf(const QString& sFn)
	{
		QString sTitle;
		if(!sFn.isEmpty()){
			sTitle = leafOf(sFn);
			int p = sTitle.lastIndexOf('.');
			if(p > 0){
				sTitle = sTitle.left(p);
			}
		}
		return sTitle;
	}

	static QString extOf(const QString& sFn, bool bDot)
	{
		QString sExt, sLeaf = leafOf(sFn);;
		if(!sLeaf.isEmpty()){
			int p = sLeaf.lastIndexOf('.');
			if(p >= 0){
				if(!bDot) p++;
				sExt = sLeaf.right(sLeaf.size()-p);
			}
		}
		return sExt;
	}

	static QString qualifyFilePath(const QString& sPath0, QChar chSep=_CHAR_SLASH)
	{
		QString sRes, sPath=sPath0;
		if(!sPath.isEmpty()){

			if(chSep!=_CHAR_SLASH) sPath.replace(_CHAR_SLASH, chSep);
			if(chSep!=_CHAR_BACKSLASH) sPath.replace(_CHAR_BACKSLASH, chSep);

			_CTextSplitter v(sPath, chSep, QString::SkipEmptyParts);
			if(!v.isEmpty()){
				//2014.4.4 consider of relative paths, like this: './abc/def/./hij/..'
				bool bRelative=(v[0]==".");
				v.removeAll(".");
				if(bRelative) v.insert(0, ".");
				sRes=v;
			}

			//Retain the leading/trailing slashes if existing;
			QChar chFirst=sPath[0], chLast=sPath[sPath.size()-1];
			if(!sRes.isEmpty()){
				if(chFirst==chSep){
					sRes=QString(chSep)+sRes;

					//2015.1.22 consider of UNC path: //t400s/mysoft/...
					if(sPath.size()>2 && sPath[1]==chSep){
						sRes=QString(chSep)+sRes;
					}
				}
			}

			if(chLast==chSep){
				sRes+=QString(chSep);
			}
		}
		return sRes;
	}

	static bool contains(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 contains the path1
		return isAncestorOf(sPath0, sPath1) || isEquivalent(sPath0, sPath1);
	}

	static bool isParentOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the parent of the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter vParent(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter vChild(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(vChild.size() != vParent.size() + 1){
			return false;
		}

		for(int i=0; i < vParent.size(); ++i){
			if(vParent[i] != vChild[i]){
				return false;
			}
		}

		return true;
	}

	static bool isChildOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is a child of path1
		return isParentOf(sPath1, sPath0);
	}

	static bool isAncestorOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the ancestor of the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter vAnce(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter vDesc(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(vAnce.size() >= vDesc.size()){
			return false;
		}

		for(int i = 0; i < vAnce.size(); ++i){
			if(vAnce[i] != vDesc[i]){
				return false;
			}
		}

		return true;
	}

	static bool isDescendantOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the descendant of the path1
		return isAncestorOf(sPath1, sPath0);
	}

	static bool isEquivalent(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is equivalent as the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter v0(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter v1(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(v1.size() != v0.size())
		{
			return false;
		}

		for(int i = 0; i < v0.size(); ++i){
			if(v0[i] != v1[i]){
				return false;
			}
		}

		return true;
	}

	static QString movePath(const QString& sPath, const QString& sFrom, const QString& sTo)
	{
		return changeRoot(sPath, sFrom, sTo);
	}

	static QString changeRoot(const QString& sPath0, const QString& sFrom0, const QString& sTo0)
	{
		QString sNewPath;
		if(!sPath0.isEmpty()){
			QString sPath = qualifyFilePath(sPath0, _CHAR_SLASH);
			QString sFrom = qualifyFilePath(sFrom0, _CHAR_SLASH); sFrom = ensureTrailingSlashes(sFrom, _CHAR_SLASH);
			QString sTo = qualifyFilePath(sTo0, _CHAR_SLASH); sTo = ensureTrailingSlashes(sTo, _CHAR_SLASH);
			if(contains(sFrom, sPath)){
				int pos = sPath.indexOf(sFrom);
				if(pos == 0){
					QString sRight = sPath.right(sPath.size() - sFrom.size());
					if(sTo.isEmpty()){
						sNewPath = sRight;
					}else{
						sNewPath = concatenate(sTo, sRight, _TEXT_SLASH);
					}
				}
			}
		}
		return sNewPath;
	}

	static QString changeExtension(const QString& sFn, const QString& sExtNew)
	{
		QString sFnNew;
		if(!sFn.isEmpty()){
			QString sDir = dirOf(sFn, true); //true: including a slash;
			QString sTitle = titleOf(sFn);
			if(!sTitle.isEmpty()){
				QString sExt=sExtNew;
				if(!sExt.isEmpty() && sExt[0]!=_CHAR_DOT){
					sExt=_TEXT_DOT+sExt;
				}
				sFnNew = sDir + sTitle + sExt;
			}
		}
		return sFnNew;
	}

	static QString relativePath(const QString& sPath0, const QString& sRoot0)
	{
		QString sRelPath;
		if(!sPath0.isEmpty() && !sRoot0.isEmpty()){
			//The 'sRoot' must be a folder path, rather than file;
			QString sRoot = ensureTrailingSlashes(sRoot0, _CHAR_SLASH);
			QString sPath = sPath0;
			sPath.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
			sRoot.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
			if(sPath.size() > sRoot.size()){
				//forcedly compare with case sensitivity;
				if(sPath.indexOf(sRoot, 0, Qt::CaseSensitive) == 0){
					sRelPath = sPath.right(sPath.size() - sRoot.size());
				}
			}
		}
		return sRelPath;
	}

	static QString fileType(const QString& sSuffix)
	{
		QString sType;
		{
#if defined(Q_OS_WIN)
			//2014.11.14 use UNICODE version of WIN32 API instead;
			QString sFileName="none."+sSuffix;
			SHFILEINFOW xSFI; memset(&xSFI, 0, sizeof(xSFI));
//			::SHGetFileInfoW(sFileName.toUtf8().constData(), FILE_ATTRIBUTE_NORMAL, &xSFI, sizeof(xSFI), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			std::vector<WCHAR> vBuf(sFileName.size()+1, 0);
			sFileName.toWCharArray(&vBuf[0]);
			::SHGetFileInfoW(&vBuf[0], FILE_ATTRIBUTE_NORMAL, &xSFI, sizeof(xSFI), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			sType = QString::fromWCharArray(&xSFI.szTypeName[0]);
#else
			QString sFileName = _CTemporaryFile()._getTempFile("", sSuffix);
			_CDeferFileDeletion dfd(sFileName); Q_UNUSED(dfd);
			if(!QFileInfo(sFileName).exists()){
				QFile f(sFileName);
				if(f.open(QIODevice::Truncate | QIODevice::WriteOnly)){
					f.close();
				}
			}
			sType = QFileIconProvider().type(QFileInfo(sFileName));
#endif
		}

		return sType;
	}


};

///////////////////////////////////////////////////////////

class _CDateTimeSmart : public QDateTime{

protected:

	QString smartString()const
	{
		QString s;
		QDate d = date();
		if(d == QDate::currentDate()){
			QTime t=time();
			//s=t.toString(Qt::TextDate); //that's: 23:59:58;
			s = t.toString("hh:mm"); //we need 23:59;
		}else{
			s = d.toString(Qt::DefaultLocaleShortDate);
		}
		return s;
	}

public:

	_CDateTimeSmart(const QDateTime& dt) : QDateTime(dt){return;}

	operator QString()const{return smartString();}

};

///////////////////////////////////////////////////////////

class _CPixmapEx{

protected:

	QPixmap			m_xPixmap;
	std::vector<QColor>	m_vBackgrounds;

protected:

	void initBackgrounds()
	{
		m_vBackgrounds.push_back(Qt::white);
		m_vBackgrounds.push_back(Qt::black);
		m_vBackgrounds.push_back(Qt::red);
		m_vBackgrounds.push_back(Qt::green);
		m_vBackgrounds.push_back(Qt::blue);
		m_vBackgrounds.push_back(Qt::yellow);
		m_vBackgrounds.push_back(Qt::cyan);
		m_vBackgrounds.push_back(Qt::magenta);
		m_vBackgrounds.push_back(QColor(255, 127, 39));
		m_vBackgrounds.push_back(QColor(181, 230, 29));
		m_vBackgrounds.push_back(QColor(0, 162, 232));
	}

public:

	_CPixmapEx()
	{
		initBackgrounds();
	}

	_CPixmapEx(const QPixmap& xPixmap)
	{
		m_xPixmap = xPixmap;
		initBackgrounds();
	}

	_CPixmapEx(const QImage& xImage)
	{
		m_xPixmap = QPixmap::fromImage(xImage);
		initBackgrounds();
	}

	_CPixmapEx(const QString& sPath)
	{
		load(sPath);
		initBackgrounds();
	}

	void load(const QString& sPath)
	{
		m_xPixmap.load(sPath);
	}

	void loadFromSvg(const QString& sPath, const QSize& szDemand)
	{
		bool bUseHighDpi = QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps);
		QSize szActual = bUseHighDpi ? szDemand * 2 : szDemand;
		m_xPixmap = QPixmap(szActual); m_xPixmap.fill(Qt::transparent);
		if(bUseHighDpi) m_xPixmap.setDevicePixelRatio(2.0);
		QPainter xPainter(&m_xPixmap); QSvgRenderer xRenderer(sPath);
		xRenderer.render(&xPainter, QRectF(0.0, 0.0, (double)szDemand.width(), (double)szDemand.height()));
	}

	static QPixmap pixmapFromSvg(const QString &sPath, const QSize &szDemand)
	{
		_CPixmapEx xPE; xPE.loadFromSvg(sPath, szDemand);
		return xPE.pixmap();
	}

	const QPixmap& pixmap() const
	{
		return m_xPixmap;
	}

	static void makeTransparent(QPixmap& xPixmap)
	{
		QColor clEample;
		{
			QImage xImage = xPixmap.toImage();
			if(xImage.height() > 0){
				QRgb xExampleRgb = xImage.pixel(0, xImage.height() - 1);
				clEample.setRgb(xExampleRgb);
			}
		}
		QBitmap xMask = xPixmap.createMaskFromColor(clEample, Qt::MaskInColor);
		xPixmap.setMask(xMask);
	}

	void makeTransparent()
	{
		QColor clEample(255, 255, 255);
		{
			QImage xImage = m_xPixmap.toImage();
			if(xImage.height() > 0){
				QRgb xExampleRgb = xImage.pixel(0, xImage.height() - 1);
				clEample.setRgb(xExampleRgb);
			}
		}
		QBitmap xMask = m_xPixmap.createMaskFromColor(clEample, Qt::MaskInColor);
		m_xPixmap.setMask(xMask);
	}

	//For transparent pixmap, and fill with a color that is different form each pixel in the pixmap
	void fillWithAutoColor()
	{
		QColor clBackground(255, 255, 255);
		QImage xImg = m_xPixmap.toImage();
		std::vector<QColor> vColors;
		for(int x = 0; x < xImg.width(); x++){
			for(int y = 0; y < xImg.height(); y++){
				QRgb xRgb = xImg.pixel(x, y);
				vColors.push_back(QColor(xRgb));
			}
		}
		std::vector<QColor>::const_iterator it = m_vBackgrounds.begin();
		for(; it != m_vBackgrounds.end(); it++){
			QColor cl = *it;
			if(std::find(vColors.begin(), vColors.end(), cl) == vColors.end()){
				clBackground = cl;
				break;
			}
		}
		m_xPixmap.fill(clBackground);
		QPainter xPainter(&m_xPixmap);
		xPainter.drawImage(0, 0, xImg);
	}

	static void setPixmapOpacity(QPixmap& xPixmap, int nOpacity)
	{
#if 1
		QPixmap xRes(xPixmap.size()); xRes.fill(Qt::transparent);
		QPainter xPainter(&xRes);
		xPainter.setOpacity((qreal)nOpacity / 255.0);
		xPainter.drawPixmap(0, 0, xPixmap);
		xPixmap = xRes;
#else
		xPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
		xPainter.fillRect(xPixmap.rect(), QColor(0, 0, 0, nOpacity));
#endif
	}

	void setPixmapOpacity(int nOpacity)
	{
		setPixmapOpacity(m_xPixmap, nOpacity);
	}

	QString toBase64(const QString& sType = "png") const
	{
		QByteArray vBuf;
		{
			QBuffer xBuffer(&vBuf);
			m_xPixmap.save(&xBuffer, sType.toLatin1().data());
		}
		return QString(vBuf.toBase64());
	}

};

///////////////////////////////////////////////////////////

class _CGuiUtils{

public:

	///////////////////////////////////////////////////////////

	static QSize sizeOfWidget(const QWidget* pWidget, double nWidth = 0.0, double nHeight = 0.0, bool bNotSmallerThanHint = true)
	{
		//Accept percentage, nWidth and nHeight specific percentage, give 0 for default
		//sizeHint, bNotSmallerThanHint ensure the size that calculated by this function
		//not smaller than the default sizeHint.
		//Note: the widget specified cannot override sizeHint for calling this function

		bool bResizeWidth = false;
		bool bResizeHeight = false;

		QRect rtScreen = QApplication::desktop()->availableGeometry();
		int w = (nWidth > 0.0) ? (int)((double)(rtScreen.width()) * nWidth) : pWidget->sizeHint().width();
		int h = (nHeight > 0.0) ? (int)((double)(rtScreen.height()) * nHeight) : pWidget->sizeHint().height();

		if(bNotSmallerThanHint){
			if(w >= pWidget->sizeHint().width()){
				bResizeWidth = true;
			}
			if(h >= pWidget->sizeHint().height()){
				bResizeHeight = true;
			}
		}else{
			bResizeWidth = true;
			bResizeHeight = true;
		}
		w = bResizeWidth ? w : pWidget->sizeHint().width();
		h = bResizeHeight ? h : pWidget->sizeHint().height();
		return QSize(w, h);
	}

	static void resizeWidgetsToSameWidth(QList<QWidget*> vWidgets, bool bMethodMax = false, int nDef = -1)
	{
		//Accept a list of widgets, resize the widgets to a size accroding to the method.
		//If the bMethodMax is true, according to the max width, otherwise according to the min width

		Q_UNUSED(nDef);
		if(!vWidgets.empty()){
			int nFixedWidth = vWidgets[0]->sizeHint().width();
			for(int i = 0; i < vWidgets.size(); i++){
				int nWidth = vWidgets[i]->sizeHint().width();
				if(bMethodMax){
					if(nFixedWidth < nWidth){
						nFixedWidth = nWidth;
					}
				}else{
					if(nFixedWidth > nWidth){
						nFixedWidth = nWidth;
					}
				}
			}
			for(int i = 0; i < vWidgets.size(); i++){
				if(bMethodMax){
					vWidgets[i]->setMinimumWidth(nFixedWidth);
				}else{
					vWidgets[i]->setMaximumWidth(nFixedWidth);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////

	static bool deselectTree(QTreeWidget* pTree)
	{
		bool bSucc = false;
		if(pTree){
			QList<QTreeWidgetItem*> vSelectedItems = pTree->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectTree(QTreeWidget* pTree)
	{
		bool bSucc = false;
		if(pTree){
			//First get current selected items
			QList<QTreeWidgetItem*> vSelectedItems = pTree->selectedItems();
			//Then select all
			pTree->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool deselectList(QListWidget* pList)
	{
		bool bSucc = false;
		if(pList){
			QList<QListWidgetItem*> vSelectedItems = pList->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectList(QListWidget* pList)
	{
		bool bSucc = false;
		if(pList){
			//First get current selected items
			QList<QListWidgetItem*> vSelectedItems = pList->selectedItems();
			//Then select all
			pList->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool deselectTable(QTableWidget* pTable)
	{
		bool bSucc = false;
		if(pTable){
			QList<QTableWidgetItem*> vSelectedItems = pTable->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectList(QTableWidget* pTable)
	{
		bool bSucc = false;
		if(pTable){
			//First get current selected items
			QList<QTableWidgetItem*> vSelectedItems = pTable->selectedItems();
			//Then select all
			pTable->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool removeWidgetFromLayout(QLayout* pLayout, int nIndex)
	{
		bool bSucc = false;
		if(pLayout){
			QLayoutItem* pLayoutItem = pLayout->itemAt(nIndex);
			if(pLayoutItem){
				pLayout->removeItem(pLayoutItem);

				delete pLayoutItem;
				pLayoutItem = NULL;

				bSucc = true;
			}
		}
		return bSucc;
	}

	static bool clearLayout(QLayout* pLayout, bool bRecursive)
	{
		bool bSucc = false;
		if(pLayout){
			QLayoutItem* pChild = NULL;
			while((pChild = pLayout->takeAt(0)) != 0){
				if(bRecursive && pChild->layout()/*If the layout item is a layout*/){
					clearLayout(pChild->layout(), bRecursive);
				}

				//2014.10.28 need to delete 'widget' as well;
				QWidget* pWidget=pChild->widget();
				if(pWidget) delete pWidget;

				delete pChild;
				pChild = NULL;
			}
			bSucc = (pLayout->count() == 0);
		}
		return bSucc;
	}

	static int indexOfWidgetInLayout(QLayout* pLayout, QWidget* pWidget)
	{
		int nRes = -1;
		if(pLayout && pWidget){
			for(int i = 0; i < pLayout->count(); i++){
				QLayoutItem* pLayoutItem = pLayout->itemAt(i);
				QWidget* pWidgetItem = pLayoutItem ? pLayoutItem->widget() : NULL;
				if(pLayoutItem && pWidgetItem && pWidgetItem == pWidget){
					nRes = i;
					break;
				}
			}
		}
		return nRes;
	}

};

///////////////////////////////////////////////////////////

class _CStandardPixmapMaker
{

public:

	static QPixmap makeColorBlock(const QColor& clColor, const QSize& szSize, Qt::BrushStyle nFillStyle = Qt::SolidPattern, bool bFrameless = false, const QColor& clBorderColor = QColor(Qt::black), Qt::PenStyle nBorderStyle = Qt::SolidLine, bool bRoundedBorder = false, qreal nRoundedRadius = 0.0)
	{
		QSize szImg = szSize; if(QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps)) szImg *= 2;
		QPixmap xImg(szImg); xImg.fill(Qt::transparent);
		if(QApplication::testAttribute(Qt::AA_UseHighDpiPixmaps)) xImg.setDevicePixelRatio(2.0);
		QPainter xPainter(&xImg);
		xPainter.setRenderHint(QPainter::Antialiasing, true);

		if(!bFrameless) xPainter.setPen(QPen(QBrush(clBorderColor), 1, nBorderStyle));
		else xPainter.setPen(QPen(Qt::NoPen));

		xPainter.setBrush(QBrush(clColor, nFillStyle));
		QRect rect(0, 0, szSize.width(), szSize.height());

		if(bRoundedBorder) xPainter.drawRoundedRect(rect, nRoundedRadius, nRoundedRadius);
		else xPainter.drawRect(rect);

		return xImg;
	}

};

///////////////////////////////////////////////////////////

class _CXmlUtils{

public:

	template<typename _CPred> static void travelDomChildElements(QDomElement& xDomEle, _CPred& xPred)
	{
		QDomNodeList vNodeList = xDomEle.childNodes();
		for(int i = 0; i < vNodeList.size(); i++){
			QDomNode xDomNode = vNodeList.item(i);
			if(xDomNode.isElement()){
				QDomElement xDomChildEle = xDomNode.toElement();
				xPred(xDomChildEle);
				travelDomChildElements(xDomChildEle, xPred);
			}
		}
	}

};

///////////////////////////////////////////////////////////////////////////////

#endif // MISC_UTILITIES_H
