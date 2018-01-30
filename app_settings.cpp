#include "app_settings.h"

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QColor& xRes, const QColor& xDef)
{
//	xRes = xDef;

	QString sDef;
	if(xDef.isValid()) sDef = (xDef.alpha() == 0) ? "transparent" : xDef.name();

	QString sVal; iniGet(xKV, sKey, sVal, sDef);
	if(!sVal.isEmpty()){
		if(sVal == "transparent"){
			xRes = QColor(Qt::transparent);
		}else{
			xRes.setNamedColor(sVal);
		}
	}else{
		xRes = QColor();
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QColor& xVal)
{
	QString sVal;

	if(xVal.isValid()) sVal = (xVal.alpha() == 0) ? "transparent" : xVal.name();

	if(!sVal.isEmpty()){
		iniPut(xKV, sKey, sVal);
	}
}

///////////////////////////////////////////////////////////

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QByteArray& xRes, const QByteArray& xDef)
{
	xRes = xDef;
	QString sVal; iniGet(xKV, sKey, sVal, "");
	if(!sVal.isEmpty()){
		xRes=QByteArray::fromBase64(sVal.toLatin1());
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QByteArray& xVal)
{
	QString sVal(xVal.toBase64());
	iniPut(xKV, sKey, sVal);
}

///////////////////////////////////////////////////////////

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QPoint& xRes, const QPoint& xDef)
{
	xRes = xDef;
	QString sVal; iniGet(xKV, sKey, sVal, "");
	if(!sVal.isEmpty()){
		_CTextSplitter v(sVal, ","); int i=0;
		int x=v(i++).toInt();
		int y=v(i++).toInt();
		xRes.setX(x);
		xRes.setY(y);
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QPoint& xVal)
{
	QString sVal;
	{
		_CTextSplitter v("", ",");
		v << QString::number(xVal.x());
		v << QString::number(xVal.y());
		sVal=(QString)v;
	}
	iniPut(xKV, sKey, sVal);
}

///////////////////////////////////////////////////////////

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QSize& xRes, const QSize& xDef)
{
	xRes = xDef;
	QString sVal; iniGet(xKV, sKey, sVal, "");
	if(!sVal.isEmpty()){
		_CTextSplitter v(sVal, ","); int i=0;
		int w=v(i++).toInt();
		int h=v(i++).toInt();
		xRes.setWidth(w);
		xRes.setHeight(h);
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QSize& xVal)
{
	QString sVal;
	{
		_CTextSplitter v("", ",");
		v << QString::number(xVal.width());
		v << QString::number(xVal.height());
		sVal=(QString)v;
	}
	iniPut(xKV, sKey, sVal);
}

///////////////////////////////////////////////////////////

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QRect& xRes, const QRect& xDef)
{
	xRes = xDef;
	QString sVal; iniGet(xKV, sKey, sVal, "");
	if(!sVal.isEmpty()){
		_CTextSplitter v(sVal, ","); int i=0;
		int x=v(i++).toInt();
		int y=v(i++).toInt();
		int w=v(i++).toInt();
		int h=v(i++).toInt();
		xRes.setRect(x, y, w, h);
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QRect& xVal)
{
	QString sVal;
	{
		_CTextSplitter v("", ",");
		v << QString::number(xVal.x());
		v << QString::number(xVal.y());
		v << QString::number(xVal.width());
		v << QString::number(xVal.height());
		sVal=(QString)v;
	}
	iniPut(xKV, sKey, sVal);
}

///////////////////////////////////////////////////////////

void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QFont& xFont, const QFont& xDef)
{
	xFont = xDef;
	if(!sKey.isEmpty()){
		QString sFamily; iniGet(xKV, sKey+".FaceName", sFamily, xDef.family()); xFont.setFamily(sFamily);
		double nSize; iniGet(xKV, sKey+".FontSize", nSize, xDef.pointSizeF()); xFont.setPointSizeF(nSize);
		int nWeight; iniGet(xKV, sKey+".Weight", nWeight, xDef.weight()); xFont.setWeight(nWeight);
		bool bItalic; iniGet(xKV, sKey+".Italic", bItalic, xDef.italic()); xFont.setItalic(bItalic);
		bool bUnderline; iniGet(xKV, sKey+".Underline", bUnderline, xDef.underline()); xFont.setUnderline(bUnderline);
		bool bStrikeOut; iniGet(xKV, sKey+".StrikeOut", bStrikeOut, xDef.strikeOut()); xFont.setStrikeOut(bStrikeOut);
	}
}

void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QFont& xFont)
{
	if(!sKey.isEmpty()){
		QString sFamily = xFont.family(); iniPut(xKV, sKey+".FaceName", sFamily);
		double nSize = xFont.pointSizeF(); iniPut(xKV, sKey+".FontSize", (double)nSize);
		int nWeight = xFont.weight(); iniPut(xKV, sKey+".Weight", (int)nWeight);
		bool bItalic = xFont.italic(); iniPut(xKV, sKey+".Italic", (bool)bItalic);
		bool bUnderline = xFont.underline(); iniPut(xKV, sKey+".Underline", (bool)bUnderline);
		bool bStrikeOut = xFont.strikeOut(); iniPut(xKV, sKey+".StrikeOut", (bool)bStrikeOut);
	}
}

///////////////////////////////////////////////////////////

_CAppSettings::_CAppSettings()
{
	_init();
}

void _CAppSettings::_init()
{
	;
}

void _CAppSettings::_ddx(_CKeyValEntries& xKV, bool bGet)
{
	iniDdx(xKV, bGet, "MRU.Files", m_vMruFiles);

	iniDdx(xKV, bGet, "Dlg.ScaleImage.KeepRatio", m_bDlgScale_KeepAspectRatio, true);

	iniDdx(xKV, bGet, "Find.Options.Case", m_bFindTextWithCase, false);
	iniDdx(xKV, bGet, "Find.Options.WholeWord", m_bFindTextWholeWord, false);
	iniDdx(xKV, bGet, "Find.Options.RegExp", m_bFindTextRegExp, false);
	iniDdx(xKV, bGet, "Find.Options.Highlight", m_bFindTextHighlightAll, true);

	iniDdx(xKV, bGet, "App.Path.OpenFile", m_sPathToOpenFile, QString(""));
	iniDdx(xKV, bGet, "App.Path.InsImage", m_sPathToInsImage, QString(""));
}

void _CAppSettings::_validate_settings()
{
	m_vColorsList[QString("Black")] = QColor(0, 0, 0);
	m_vColorsList[QString("Brown")] = QColor(153, 52, 0);
	m_vColorsList[QString("Olive")] = QColor(51, 51, 0);
	m_vColorsList[QString("Dark Green")] = QColor(0, 51, 0);
	m_vColorsList[QString("Deep Grayish Blue")] = QColor(0, 51, 102);
	m_vColorsList[QString("Dark Blue")] = QColor(0, 0, 128);
	m_vColorsList[QString("Indigo")] = QColor(51, 51, 153);
	m_vColorsList[QString("Dark Gray")] = QColor(51, 51, 51);

	m_vColorsList[QString("Dark Red")] = QColor(128, 0, 0);
	m_vColorsList[QString("Orange Red")] = QColor(255, 102, 0);
	m_vColorsList[QString("Dark Yellow")] = QColor(128, 128, 0x00);
	m_vColorsList[QString("Green")] = QColor(0, 128, 0);
	m_vColorsList[QString("Cyan")] = QColor(0, 128, 128);
	m_vColorsList[QString("Blue")] = QColor(0, 0, 255);
	m_vColorsList[QString("Blue Ash")] = QColor(102, 102, 153);
	m_vColorsList[QString("Gray")] = QColor(128, 128, 128);

	m_vColorsList[QString("Red")] = QColor(255, 0, 0);
	m_vColorsList[QString("Orange")] = QColor(255, 153, 0);
	m_vColorsList[QString("Lime")] = QColor(153, 204, 0);
	m_vColorsList[QString("Sea Green")] = QColor(51, 153, 102);
	m_vColorsList[QString("Sapphire")] = QColor(51, 204, 204);
	m_vColorsList[QString("Light Blue")] = QColor(51, 102, 255);
	m_vColorsList[QString("Purple")] = QColor(128, 0, 128);
	m_vColorsList[QString("Light Gray")] = QColor(153, 153, 153);

	m_vColorsList[QString("Fuchsia")] = QColor(255, 0, 255);
	m_vColorsList[QString("Gold")] = QColor(255, 204, 0);
	m_vColorsList[QString("Yellow")] = QColor(255, 255, 0);
	m_vColorsList[QString("Bright Green")] = QColor(0, 255, 0);
	m_vColorsList[QString("Aqua")] = QColor(0, 255, 255);
	m_vColorsList[QString("Sky Blue")] = QColor(0, 204, 255);
	m_vColorsList[QString("Plum")] = QColor(153, 51, 102);
	m_vColorsList[QString("Silver")] = QColor(192, 192, 192);

	m_vColorsList[QString("Pink")] = QColor(255, 153, 204);
	m_vColorsList[QString("Light Brown")] = QColor(255, 204, 153);
	m_vColorsList[QString("Light Yellow")] = QColor(255, 255, 153);
	m_vColorsList[QString("Light Green")] = QColor(204, 255, 204);
	m_vColorsList[QString("Light Aqua")] = QColor(204, 255, 255);
	m_vColorsList[QString("Pale Blue")] = QColor(153, 204, 255);
	m_vColorsList[QString("Pale Purple")] = QColor(204, 153, 255);
	m_vColorsList[QString("White")] = QColor(255, 255, 255);
}

void _CAppSettings::load(const QString& sIniFn)
{
	//load settings from INI;
	{
		QString sTxt=_CTextFile(sIniFn).load("");
		_CKeyValEntries xKV(sTxt);

		//Must run this function for paramater initialization, no matter if INI file is empty or not;
		bool bGet=true; _ddx(xKV, bGet);
	}

	//locale messages;
//	loadLangIni();

	//finally validate certain settings;
	_validate_settings();
}

void _CAppSettings::saveIni(const QString& sIniFn)
{
	QString sTxt;
	{
		_CKeyValEntries xKV; bool bGet=false; _ddx(xKV, bGet); sTxt=xKV;
	}
	_CTextFile(sIniFn).saveUtf8(sTxt, true);
}
