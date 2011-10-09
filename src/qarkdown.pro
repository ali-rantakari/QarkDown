HEADERS = \
    defines.h\
    qarkdownapplication.h \
    mainwindow.h \
    preferencesdialog.h \
    peg-markdown-highlight/pmh_parser.h \
    peg-markdown-highlight/pmh_definitions.h \
    peg-markdown-highlight/highlighter.h \
    editor/qarkdowntextedit.h \
    editor/linenumberingplaintextedit.h \
    peg-markdown-highlight/pmh_styleparser.h \
    markdowncompiler.h \
    logger.h
SOURCES = \
    main.cpp \
    qarkdownapplication.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    peg-markdown-highlight/pmh_parser.c\
    peg-markdown-highlight/highlighter.cpp \
    editor/qarkdowntextedit.cpp \
    editor/linenumberingplaintextedit.cpp \
    peg-markdown-highlight/pmh_styleparser.c \
    markdowncompiler.cpp \
    logger.cpp

FORMS += \
    preferencesdialog.ui

OTHER_FILES += \
    cfg/win.rc \
    cfg/Info.plist \
    cfg/linux.qrc \
    template.html

win32 {
    RC_FILE += cfg/win.rc
    RESOURCES += compilers-windows.qrc
}

macx {
    RC_FILE += gfx/icon.icns
    QMAKE_INFO_PLIST = cfg/Info.plist
    RESOURCES += compilers-osx.qrc
}

linux-g++ {
    RESOURCES += cfg/linux.qrc
    RESOURCES += compilers-linux.qrc
}

RESOURCES += \
    styles.qrc \
    misc.qrc

CONFIG(release, debug|release) {
    DEFINES += BUILD_RELEASE
}
CONFIG(debug, debug|release) {
    DEFINES += BUILD_DEBUG
}
