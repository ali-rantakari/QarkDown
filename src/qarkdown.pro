HEADERS = \
    defines.h\
    qarkdownapplication.h \
    mainwindow.h \
    preferencesdialog.h \
    peg-markdown-highlight/markdown_parser.h \
    peg-markdown-highlight/markdown_definitions.h \
    peg-markdown-highlight/highlighter.h \
    editor/qarkdowntextedit.h \
    editor/linenumberingplaintextedit.h \
    peg-markdown-highlight/styleparser.h \
    markdowncompiler.h
SOURCES = \
    main.cpp \
    qarkdownapplication.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    peg-markdown-highlight/markdown_parser.c\
    peg-markdown-highlight/highlighter.cpp \
    editor/qarkdowntextedit.cpp \
    editor/linenumberingplaintextedit.cpp \
    peg-markdown-highlight/styleparser.c \
    markdowncompiler.cpp

FORMS += \
    preferencesdialog.ui

OTHER_FILES += \
    cfg/win.rc \
    cfg/Info.plist \
    cfg/linux.qrc

win32 {
    RC_FILE += cfg/win.rc
    RESOURCES += compilers-windows.qrc
}

macx {
    RC_FILE += gfx/icon.icns
    QMAKE_INFO_PLIST = cfg/Info.plist
    RESOURCES += compilers-osx.qrc
}

linux {
    RESOURCES += cfg/linux.qrc
    RESOURCES += compilers-linux.qrc
}

RESOURCES += \
    cfg/linux.qrc \
    styles.qrc \
    compilers-windows.qrc \
    compilers-osx.qrc \
    compilers-linux.qrc
