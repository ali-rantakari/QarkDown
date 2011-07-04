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
    peg-markdown-highlight/styleparser.h
SOURCES = \
    main.cpp \
    qarkdownapplication.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    peg-markdown-highlight/markdown_parser.c\
    peg-markdown-highlight/highlighter.cpp \
    editor/qarkdowntextedit.cpp \
    editor/linenumberingplaintextedit.cpp \
    peg-markdown-highlight/styleparser.c

FORMS += \
    preferencesdialog.ui

OTHER_FILES += \
    cfg/win.rc \
    cfg/Info.plist \
    cfg/linux.qrc

win32 {
    RC_FILE += cfg/win.rc
}

macx {
    RC_FILE += gfx/icon.icns
    QMAKE_INFO_PLIST = cfg/Info.plist
}

linux {
    RESOURCES += cfg/linux.qrc
}

RESOURCES += \
    cfg/linux.qrc \
    styles.qrc
