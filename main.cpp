#include <QApplication>
#include "common_headers.h"
#include "MainWindow.h"

_CAppSettings g_xOpt;

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	{
		QString sIniFn = _CLocalFile::changeExtension(QCoreApplication::applicationFilePath(), "ini");
		g_xOpt.load(sIniFn);
	}

	MainWindow w;
	w.showMaximized();

	return a.exec();
}
