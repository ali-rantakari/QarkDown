HEADERS         = markdown_parser.h \
                  markdown_definitions.h \
                  highlighter.h \
    mainwindow.h \
    preferencesdialog.h \
    defines.h \
    qarkdowntextedit.h \
    qarkdownapplication.h
SOURCES         = markdown_parser.c\
                  highlighter.cpp \
                  main.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp \
    qarkdowntextedit.cpp \
    qarkdownapplication.cpp

FORMS += \
    preferencesdialog.ui

OTHER_FILES += \
    win.rc \
    Info.plist

win32:RC_FILE += win.rc

mac:RC_FILE += gfx/icon.icns
mac:QMAKE_INFO_PLIST = Info.plist
