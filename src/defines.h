#ifndef DEFAULTPREFERENCES_H
#define DEFAULTPREFERENCES_H

#define SETTING_EXTENSIONS "FileExtensions"
#define SETTING_FONT "Font"
#define SETTING_TAB_WIDTH "TabWidth"
#define SETTING_HIGHLIGHT_INTERVAL "HighlightInterval"
#define SETTING_INDENT_WITH_TABS "IndentWithTabs"
#define SETTING_REMEMBER_LAST_FILE "RememberLastOpenFile"
#define SETTING_REMEMBER_WINDOW "RememberLastWindowSizeAndPos"
#define SETTING_CLICKABLE_LINKS "ClickableLinks"
#define SETTING_HIGHLIGHT_CURRENT_LINE "HighlightCurrentLine"
#define SETTING_STYLE "HighlightingStyle"

#define SETTING_LAST_FILE "LastFile"
#define SETTING_WINDOW_GEOMETRY "LastWindowGeometry"
#define SETTING_WINDOW_STATE "LastWindowState"

#define SETTING_RECENT_FILES "RecentFiles"
#define SETTING_NUM_RECENT_FILES "NumberOfRecentFiles"

#define SETTING_COMPILER "Compiler"
#define SETTING_OPEN_TARGET_AFTER_COMPILING "OpenTargetAfterCompiling"

#define DEF_EXTENSIONS          "mdtext md markdown txt text"
#define DEF_FONT_FAMILY         "Courier"
#define DEF_FONT_SIZE           12
#define DEF_TAB_WIDTH           4
#define DEF_HIGHLIGHT_INTERVAL  0.5
#define DEF_NUM_RECENT_FILES    10
#define DEF_INDENT_WITH_TABS    false
#define DEF_REMEMBER_LAST_FILE  false
#define DEF_REMEMBER_WINDOW     true
#define DEF_CLICKABLE_LINKS     false
#define DEF_HIGHLIGHT_CURRENT_LINE true
#define DEF_LINE_HIGHLIGHT_COLOR QColor(Qt::yellow).lighter(180)
#define DEF_STYLE               ":/styles/Default"
#define DEF_OPEN_TARGET_AFTER_COMPILING true

#ifdef Q_WS_MACX
#define DEF_COMPILER            ":/compilers/multimarkdown/multimarkdown-osx"
#elif defined(Q_WS_WIN)
#define DEF_COMPILER            ":/compilers/peg-markdown/peg-markdown-windows.exe"
#elif defined(Q_WS_LINUX)
#define DEF_COMPILER            ":/compilers/peg-markdown/peg-markdown-linux"
#else
#define DEF_COMPILER            ""
#endif

#define HTML_TEMPLATE_FILE_PATH \
    QDir(((QarkdownApplication *)qApp)->applicationStoragePath()\
         + "/template.html").absolutePath()

#endif // DEFAULTPREFERENCES_H
