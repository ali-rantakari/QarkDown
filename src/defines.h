#ifndef DEFAULTPREFERENCES_H
#define DEFAULTPREFERENCES_H

#define MARKDOWN_FILES_FILTER "Markdown Files (*.mdtext *.md *.markdown *.txt *.text)"

#define SETTING_FONT "Font"
#define SETTING_TAB_WIDTH "TabWidth"
#define SETTING_HIGHLIGHT_INTERVAL "HighlightInterval"
#define SETTING_INDENT_WITH_TABS "IndentWithTabs"
#define SETTING_REMEMBER_LAST_FILE "RememberLastOpenFile"
#define SETTING_REMEMBER_WINDOW "RememberLastWindowSizeAndPos"
#define SETTING_CLICKABLE_LINKS "ClickableLinks"
#define SETTING_HIGHLIGHT_CURRENT_LINE "HighlightCurrentLine"
#define SETTING_LINE_HIGHLIGHT_COLOR "LineHighlightColor"
#define SETTING_STYLE "HighlightingStyle"

#define SETTING_LAST_FILE "LastFile"
#define SETTING_WINDOW_GEOMETRY "LastWindowGeometry"
#define SETTING_WINDOW_STATE "LastWindowState"

#define DEF_FONT_FAMILY         "Courier"
#define DEF_FONT_SIZE           12
#define DEF_TAB_WIDTH           4
#define DEF_HIGHLIGHT_INTERVAL  0.5
#define DEF_INDENT_WITH_TABS    false
#define DEF_REMEMBER_LAST_FILE  false
#define DEF_REMEMBER_WINDOW     true
#define DEF_CLICKABLE_LINKS     false
#define DEF_HIGHLIGHT_CURRENT_LINE true
#define DEF_LINE_HIGHLIGHT_COLOR QColor(Qt::yellow).lighter(180)
#define DEF_STYLE               ":/styles/default"

#endif // DEFAULTPREFERENCES_H
