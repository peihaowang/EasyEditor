#ifndef _CAPPSETTINGS_H
#define _CAPPSETTINGS_H

#include <ctime>
#include <map>
#include <iterator>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QAction>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QByteArray>
#include <QMap>
#include "platform_specific.h"
#include "cpp_utilities.h"
#include "string_utilities.h"
#include "file_utilities.h"

class _CAppSettings
{

public:

	_CPairVector<QString, QColor>	m_vColorsList;

	QStringList			m_vMruFiles;

	bool				m_bDlgScale_KeepAspectRatio;

	bool				m_bFindTextWithCase;
	bool				m_bFindTextWholeWord;
	bool				m_bFindTextHighlightAll;
	bool				m_bFindTextRegExp;

	QString				m_sPathToOpenFile;
	QString				m_sPathToExportFile;
	QString				m_sPathToInsImage;

	QSize				m_szIconSize;

protected:

	void _init(void);
	void _ddx(_CKeyValEntries& xKV, bool bGet);
	void _validate_settings(void);

public:

	_CAppSettings();

	void load(const QString& sIniFn);
	void saveIni(const QString& sIniFn);

	QIcon getIconForToolBar(const QString& sFilename);

};

extern _CAppSettings g_xOpt;

#endif // _CAPPSETTINGS_H
