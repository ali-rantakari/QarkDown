HEADERS         = markdown_parser.h \
                  markdown_definitions.h \
                  highlighter.h \
    mainwindow.h \
    preferencesdialog.h \
    defines.h \
    qarkdownapplication.h \
    editor/qarkdowntextedit.h \
    editor/linenumberingplaintextedit.h
SOURCES         = markdown_parser.c\
                  highlighter.cpp \
                  main.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    qarkdownapplication.cpp \
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
