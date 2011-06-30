HEADERS = \
    defines.h\
    qarkdownapplication.h \
    mainwindow.h \
    preferencesdialog.h \
    peg-markdown-highlight/markdown_parser.h \
    peg-markdown-highlight/markdown_definitions.h \
    peg-markdown-highlight/highlighter.h \
    editor/qarkdowntextedit.h \
    editor/linenumberingplaintextedit.h
SOURCES = \
    main.cpp \
    qarkdownapplication.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    peg-markdown-highlight/markdown_parser.c\
    peg-markdown-highlight/highlighter.cpp \
    editor/qarkdowntextedit.cpp \
    editor/linenumberingplaintextedit.cpp

FORMS += \
    preferencesdialog.ui

OTHER_FILES += \
    win.rc \
    Info.plist

win32:RC_FILE += win.rc

mac:RC_FILE += gfx/icon.icns
mac:QMAKE_INFO_PLIST = Info.plist
