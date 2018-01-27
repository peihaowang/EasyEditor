#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <map>
#include <iterator>

#include <QDebug>
#include <QStringList>
#include <QString>

#include "platform_specific.h"

///////////////////////////////////////////////////////////

class _CPredStrList : public QStringList{

public:

	void operator()(const QString& t){push_back(t);}

};

///////////////////////////////////////////////////////////

class _CTextSplitter : public QStringList{

private:

	QString				m_sSeparator;
	QString::SplitBehavior		m_iBehavior;
	Qt::CaseSensitivity		m_iCase;

public:

	_CTextSplitter(const QString& sTxt, const QString& sSep, QString::SplitBehavior behavior = QString::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive)
		: m_sSeparator(sSep)
		, m_iBehavior(behavior)
		, m_iCase(cs)
	{
		*this=sTxt;
	}

	_CTextSplitter(const QStringList& vSrc, const QString& sSep, QString::SplitBehavior behavior = QString::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive)
		: m_sSeparator(sSep)
		, m_iBehavior(behavior)
		, m_iCase(cs)
	{
		*this << vSrc;
	}

	QString operator()(int i, const QString& sDef="")const{return (i>=0 && i<this->size()) ? this->at(i) : sDef;}

	operator QString()const{return toString();}

	const _CTextSplitter& operator=(const QString& sTxt)
	{
		//2014.2.17 Caution: An empty string also insert one into the QStringList;
		//DOC: If sep does not match anywhere in the string, split() returns a single-element list containing this string.
		QStringList::clear();
		if(!sTxt.isEmpty()){
			QStringList& self=*this;
			self=sTxt.split(m_sSeparator, m_iBehavior, m_iCase);
		}
		return *this;
	}

	const _CTextSplitter& operator=(const QList<QString>& v)
	{
		for(int j = 0; j < v.size(); j++){
			*this << v[j];
		}
		return *this;
	}

	QString toString(void)const
	{
		QString sRes;
		{
			QStringList::const_iterator it;
			for(it=begin(); it!=end(); ++it){
				if(!sRes.isEmpty()) sRes+=m_sSeparator;
				sRes+=*it;
			}
		}
		return sRes;
	}
};

///////////////////////////////////////////////////////////

class _CKeyValEntries : public std::map<QString, QString>{

private:

	QString			m_sEOL;

private:

	static QString _trim_right(const QString& s, QChar chToTrim)
	{
		QString sRes=s;
		int c=s.size(), nTrim=0;
		if(c>0){
			for(int i=c-1; i>=0; --i){
				if(s[i]!=chToTrim) break;
				nTrim++;
			}
			if(nTrim==c) sRes="";
			else if(nTrim>0) sRes=s.left(c-nTrim);
		}
		return sRes;
	}

	//static QString _makeKey(const QString& sKey){return sKey.trimmed().toUpper();}
	static QString _makeKey(const QString& sKey){return sKey.trimmed();}
	static QString _makeVal(const QString& sVal){return _trim_right(sVal, '\r');}

	int _unserialize(const QString& sTxt)
	{
		int nDone=0;
//		qDebug() << "->_CKeyValEntries::_unserialize(nTxtLen=" << sTxt.size() << ")";
		QString sEOL=m_sEOL; if(sEOL.isEmpty()) sEOL="\n";
		QStringList vLines=sTxt.split(sEOL, QString::SkipEmptyParts);
		QStringList::const_iterator it;
		for(it=vLines.begin(); it!=vLines.end(); ++it){
			QString sLine=*it;
			int iPos=sLine.indexOf("=");
			if(iPos>0){
				QString sKey=_makeKey(sLine.left(iPos));
				QString sVal=_makeVal(sLine.mid(iPos+1));
				this->insert(std::pair<QString, QString>(sKey, sVal));
				nDone++;
			}
		}
		return nDone;
	}

	QString _serialize(void)const
	{
		QString sRes;
		QString sEOL=m_sEOL; if(sEOL.isEmpty()) sEOL=_END_OF_LINE;
		std::map<QString, QString>::const_iterator it;
		for(it=begin(); it!=end(); ++it){
			if(!sRes.isEmpty()) sRes+=sEOL;
			sRes+=(it->first+"="+it->second);
		}
		return sRes;
	}

public:

	_CKeyValEntries(const QString& sTxt="", const QString& sEOL="")
		: m_sEOL(sEOL)
	{
		if(!sTxt.isEmpty()) _unserialize(sTxt);
	}

	int operator=(const QString& sTxt){return _unserialize(sTxt);}
	operator QString()const{return _serialize();}

	QString operator()(const QString& sKey, const QString& sDef="")const
	{
		QString sVal; get(sKey, sVal, sDef);
		return sVal;
	}

	bool get(const QString& sKey, QString& sVal, const QString& sDef)const
	{
		bool bSucc=false;
		sVal = sDef; //2014.2.20 forcedly initialize it with the default value;
		QString k=_makeKey(sKey);
		if(!k.isEmpty()){
			std::map<QString, QString>::const_iterator it=this->find(k);
			if(it!=this->end()){
				sVal = it->second;
				bSucc=true;
			}
		}
//qDebug() << "->get: " << k << "=" << sVal << ", " << (bSucc ? "OK" : "DEF");
		return bSucc;
	}

	bool put(const QString& sKey, const QString& sVal)
	{
		bool bSucc=false;
		QString k=_makeKey(sKey);
		QString v=_makeVal(sVal);
		if(!k.isEmpty()){
			//2014.9.11 'insert' does not change values of existing 'key' entries;
			//http://stackoverflow.com/questions/326062/in-stl-maps-is-it-better-to-use-mapinsert-than
			//this->insert(std::pair<QString, QString>(k, v));
			std::map<QString, QString>& m=*this;
			m[k]=v; //2014.9.11 forcedly overwritten;
			bSucc=true;
		}
//qDebug() << "->put: " << k << "=" << sVal << ", " << (bSucc ? "OK" : "FAIL");
		return bSucc;
	}

	bool del(const QString& sKey)
	{
		bool bSucc=false;
		std::map<QString, QString>& m=*this;
		if(!sKey.isEmpty()){
			if(m.erase(sKey)>0){
				bSucc=true;
			}
		}
		return bSucc;
	}
};

///////////////////////////////////////////////////////////

template<typename T> void iniDdx(_CKeyValEntries& xKV, bool bGet, const QString& sKey, T& xVal, const T& xDef)
{
	//2014.2.19 for primitive data types;
	if(bGet){
		iniGet(xKV, sKey, xVal, xDef);
	}else{
		iniPut(xKV, sKey, xVal);
	}
}

template<typename T> void iniDdx(_CKeyValEntries& xKV, bool bGet, const QString& sKey, T& xVal)
{
	//2014.2.19 for stl containers such as std::vector;
	if(bGet){
		iniGet(xKV, sKey, xVal);
	}else{
		iniPut(xKV, sKey, xVal);
	}
}

///////////////////////////////////////////////////////////

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QString& sVal, const QString& sDef)
{
	sVal=xKV(sKey, sDef);
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, int& nRes, const int& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toInt();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, unsigned int& nRes, const unsigned int& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toUInt();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, long& nRes, const long& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toLong();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, unsigned long& nRes, const unsigned int& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toULong();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, qlonglong& nRes, const qlonglong& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toLongLong();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, qulonglong& nRes, const qulonglong& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toULongLong();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, double& nRes, const double& nDef)
{
	nRes = nDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		nRes = sVal.toDouble();
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, bool& bRes, const bool& bDef)
{
	bRes = bDef;
	QString sVal=xKV(sKey);
	if(!sVal.isEmpty()){
		bRes = sVal.toInt()!=0;
	}
}

///////////////////////////////////////////////////////////

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QString& sVal)
{
	xKV.put(sKey, sVal);
}

#if 1
template<typename TNum> void iniPut(_CKeyValEntries& xKV, const QString& sKey, const TNum& nVal)
{
	QString sVal = QString("%1").arg((qlonglong)nVal);
	xKV.put(sKey, sVal);
}
#else
inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const int& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const unsigned int& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const long& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const unsigned long& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const qlonglong& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const qulonglong& nVal)
{
	QString sVal = QString("%1").arg(nVal);
	xKV.put(sKey, sVal);
}
#endif

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const bool& bVal)
{
	QString sVal = bVal?"1":"0";
	xKV.put(sKey, sVal);
}

///////////////////////////////////////////////////////////

template<typename T> void iniPut(_CKeyValEntries& xKV, const QString& sKey, const std::vector<T>& v)
{
	typename std::vector<T>::size_type nCount = v.size();
	iniPut(xKV, sKey+".Count", (int)nCount);
	typename std::vector<T>::const_iterator it;
	for(it=v.begin(); it!=v.end(); ++it){
		int i=std::distance(v.begin(), it);
		QString k=QString("%1.Item[%2]").arg(sKey).arg(i);
		iniPut(xKV, k, *it);
	}
}

template<typename T> void iniGet(const _CKeyValEntries& xKV, const QString& sKey, std::vector<T>& v)
{
	v.clear();
	int nCount=-1; iniGet(xKV, QString("%1.Count").arg(sKey), nCount, (int)-1);
	for(int i = 0; i < nCount; i++){
		T xDef = T();
		T xRes; iniGet(xKV, QString("%1.Item[%2]").arg(sKey).arg(i), xRes, xDef);
		v.push_back(xRes);
	}
}

//template<typename T> void iniGet(const _CKeyValEntries& xKV, const QString& sKey, std::vector<T>& v, const std::vector<T>& vDef)
//{
//	v.clear();
//	int nCount; iniGet(xKV, QString("%1.Count").arg(sKey), nCount, (int)-1);
//	if(nCount >= 0){
//		iniGet(xKV, sKey, v);
//	}else{
//		v = vDef;
//	}
//}

///////////////////////////////////////////////////////////

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QStringList& v)
{
	iniPut(xKV, sKey+".Count", (int)v.size());
	int i=0;
	Q_FOREACH(QString s, v){
		QString k=QString("%1.Item[%2]").arg(sKey).arg(i++);
		iniPut(xKV, k, s);
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QStringList& v)
{
	v.clear();
	int nCount=-1; iniGet(xKV, QString("%1.Count").arg(sKey), nCount, (int)-1);
	for(int i = 0; i < nCount; i++){
		QString sTmp; iniGet(xKV, QString("%1.Item[%2]").arg(sKey).arg(i), sTmp, QString(""));
		v.push_back(sTmp);
	}
}

///////////////////////////////////////////////////////////

inline void iniPut(_CKeyValEntries& xKV, const QString& sKey, const std::map<QString, QString>& m)
{
	std::map<QString, QString>::size_type nCount = m.size();
	iniPut(xKV, sKey+".Count", (int)nCount);
	std::map<QString, QString>::const_iterator it; int i=0;
	for(it=m.begin(); it!=m.end(); ++it){
		QString k=QString("%1.Item[%2]").arg(sKey).arg(i++);
		QString v=QString("%1=%2").arg(it->first).arg(it->second);
		iniPut(xKV, k, v);
	}
}

inline void iniGet(const _CKeyValEntries& xKV, const QString& sKey, std::map<QString, QString>& m)
{
	m.clear();
	int nCount=-1; iniGet(xKV, QString("%1.Count").arg(sKey), nCount, (int)-1);
	for(int i = 0; i < nCount; i++){
		QString sVal; iniGet(xKV, QString("%1.Item[%2]").arg(sKey).arg(i), sVal, QString(""));
		if(!sVal.isEmpty()){
			int p=sVal.indexOf("=");
			if(p>0){
				QString k=sVal.left(p);
				QString v=sVal.mid(p+1);
				if(!k.isEmpty() && !v.isEmpty()){
					m[k]=v;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////

#endif // STRING_UTILITIES_H
