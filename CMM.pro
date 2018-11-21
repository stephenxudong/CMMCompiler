#-------------------------------------------------
#
# Project created by QtCreator 2018-11-19T11:58:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CMM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    compiler/compiler.cpp \
    compiler/Lexical.cpp \
    compiler/Parser.cpp \
    type/Token.cpp \
    util/Thread.cpp \
    util/Util.cpp \
    compiler/Semantic.cpp \
    type/SymbolTable.cpp \
    type/TableNode.cpp

HEADERS += \
        mainwindow.h \
    compiler/compiler.h \
    compiler/Lexical.h \
    compiler/Parser.h \
    type/TableNode.h \
    type/Token.h \
    type/tokentype.h \
    type/TreeNode.h \
    type/TreeNodeType.h \
    util/Thread.h \
    util/Util.h \
    compiler/Semantic.h \
    type/SymbolTable.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    test/if.cmm \
    test/in.cmm \
    test/in1.cmm \
    test/while.cmm \
    README.md \
    out.txt
