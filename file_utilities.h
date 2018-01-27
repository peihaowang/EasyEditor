#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>

///////////////////////////////////////////////////////////

class _CTextFile{

private:

	QString			m_sFn;

public:

	static QString codecName(const QByteArray& vBuf, bool bHtml=false)
	{
		QString sName;
		QTextCodec* pDef=QTextCodec::codecForLocale();
		QTextCodec* p=bHtml ? QTextCodec::codecForHtml(vBuf, pDef) : QTextCodec::codecForUtfText(vBuf, pDef);
		if(p){
			QByteArray v=p->name();
			sName=v;
			//qDebug() << "->detectCodecName: " << sName;
		}
		return sName;
	}

	///////////////////////////////////////////////////

	static QString load(QIODevice& xIO, const QString& sCodec="")
	{
		QString sTxt;
		try{
			QTextStream ts(&xIO);
			if(!sCodec.isEmpty() && sCodec!="System") ts.setCodec(sCodec.toStdString().c_str());
			sTxt=ts.readAll();
		}catch(...){
			qDebug() << "**Error loading text. sCodec=" << sCodec;
		}
		return sTxt;
	}

	static QString load(const QByteArray& vBuf, const QString& sDefCodec)
	{
		QString sTxt, sCodec=sDefCodec;
		if(sCodec.isEmpty() || sCodec.toLower()=="auto") sCodec=codecName(vBuf); //auto-detect codec if necessary;
		QBuffer b; b.setData(vBuf);
		if(b.open(QIODevice::ReadOnly)){
			sTxt=load((QIODevice&)b, sCodec);
		}
		return sTxt;
	}

	static QString load(const QString& sFn, const QString& sDefCodec)
	{
		QString sTxt;
		{
//			qDebug() << "->loadTextFile(sFn=" << sFn << ", sCodec=" << sDefCodec << ")";
			QFile f(sFn);
			if(f.open(QIODevice::ReadOnly|QIODevice::Text)){
				int nLen=f.size();
				if(nLen>0){
					QByteArray v=f.read(nLen);
					sTxt=load(v, sDefCodec);
				}
				f.close();
			}
		}
		return sTxt;
	}

	///////////////////////////////////////////////////

	static bool save(QIODevice& xIO, const QString& sTxt, const QString& sCodec)
	{
		bool bSucc=false;
		try{
			QTextStream ts(&xIO);
			if(!sCodec.isEmpty()) ts.setCodec(sCodec.toStdString().c_str());
			ts << sTxt;
			bSucc=true;
		}catch(...){
		}
		return bSucc;
	}

	static qint64 save(const QString& sFn, const QString& sTxt, const QString& sCodec)
	{
		qint64 nBytes=-1;
		QFile f(sFn);
		if(f.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)){
			if(save(f, sTxt, sCodec)){
				nBytes=f.size();
			}
			f.close();
		}
		return nBytes;
	}

	static qint64 saveAnsi(const QString& sFn, const QString& sTxt)
	{
		QTextCodec* pDef=QTextCodec::codecForLocale();
		return save(sFn, sTxt, pDef ? pDef->name() : "");
	}

	static qint64 saveUtf8(const QString& sFn, const QString& sTxt, bool bBOM)
	{
		qint64 nBytes=-1;
		QFile f(sFn);
		if(f.open(QIODevice::WriteOnly|QIODevice::Truncate)){
			const char* _BOM_UTF8="\xEF\xBB\xBF";
			QByteArray v; if(bBOM) v.append(_BOM_UTF8);
			v.append(sTxt.toUtf8());
			f.write(v);
			nBytes=f.size();
			f.close();
		}
		return nBytes;
	}

	///////////////////////////////////////////////////

public:

	_CTextFile(const QString& sFn) : m_sFn(sFn){return;}

	QString fileName(void)const{return m_sFn;}
	void fileName(const QString& sFn){m_sFn=sFn;}

	QString load(const QString& sCodec)const
	{
		return load(m_sFn, sCodec);
	}

	qint64 save(const QString& sTxt, const QString& sCodec)const
	{
		return save(m_sFn, sTxt, sCodec);
	}
	qint64 saveAnsi(const QString& sTxt)const
	{
		return saveAnsi(m_sFn, sTxt);
	}

	qint64 saveUtf8(const QString& sTxt, bool bBOM)const
	{
		return saveUtf8(m_sFn, sTxt, bBOM);
	}

};

#endif // FILE_UTILITIES_H
