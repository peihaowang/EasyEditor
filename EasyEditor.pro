#-------------------------------------------------
#
# Project created by QtCreator 2018-01-22T19:29:39
#
#-------------------------------------------------

QT       += core gui xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EasyEditor
TEMPLATE = app


SOURCES += main.cpp\
    app_settings.cpp \
    ColorPicker.cpp \
    ComboBoxColor.cpp \
    DlgParaMargin.cpp \
    DlgScaleImg.cpp \
    MenuColor.cpp \
    MenuUnderline.cpp \
    MyRichEdit.cpp \
    PanelFindReplace.cpp \
    MyDocument.cpp \
    MainWindow.cpp \
    MacUnifiedToolBar.cpp

HEADERS  += \
    app_settings.h \
    app_utilities.h \
    ColorPicker.h \
    ComboBoxColor.h \
    common_headers.h \
    cpp_utilities.h \
    DlgParaMargin.h \
    DlgScaleImg.h \
    file_utilities.h \
    MenuColor.h \
    MenuUnderline.h \
    MyRichEdit.h \
    PanelFindReplace.h \
    platform_specific.h \
    string_utilities.h \
    type_defs.h \
    MyDocument.h \
    MainWindow.h \
    MacUnifiedToolBar.h

RESOURCES += \
    res.qrc

DISTFILES +=
