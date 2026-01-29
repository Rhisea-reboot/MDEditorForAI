QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 目标文件名称
TARGET = MDEditorGUI
TEMPLATE = app

# 包含路径
INCLUDEPATH += $$PWD/core

# 源文件
SOURCES += \
    main.cpp \
    MainWindow.cpp \
    core/getFile.cpp \
    core/initPrint.cpp

# 头文件
HEADERS += \
    MainWindow.h \
    core/getFile.h \
    core/initPrint.h

# 资源处理（如果需要图标等，此处暂留空）
