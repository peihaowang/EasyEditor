
#include <ctime>
#include <cstdlib>

#include <vector>
#include <string>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <QObject>
#include <QApplication>

#include <QtGlobal>
#include <QDebug>
#include <QTextCodec>
#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QUrl>
#include <QLibrary>

#include <QtGui>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QDateEdit>
#include <QGroupBox>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QHeaderView>
#include <QListWidget>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QFontComboBox>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#else

#include <QtWidgets>

#endif

#include "type_defs.h"

#include "platform_specific.h"
#include "cpp_utilities.h"
#include "file_utilities.h"
#include "string_utilities.h"

#include "app_utilities.h"

#include "app_settings.h"

