HEADERS         = markdown_parser.h \
                  markdown_definitions.h \
                  highlighter.h \
    mainwindow.h \
    preferencesdialog.h \
    defines.h \
    qarkdowntextedit.h
SOURCES         = markdown_parser.c\
                  highlighter.cpp \
                  main.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    qarkdowntextedit.cpp

FORMS += \
    preferencesdialog.ui

QMAKE_INFO_PLIST = Info.plist

OTHER_FILES += \
    win.rc \
    Info.plist

win32:RC_FILE += win.rc
