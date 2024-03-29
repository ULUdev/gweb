#ifndef __GWEB_TABBING_H__
#define __GWEB_TABBING_H__
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdbool.h>
#define GWEB_BOX_SPACING 2
#define GWEB_SEARCH_QUERY "http://duckduckgo.com/?q="
typedef struct GwebTabs gweb_tabs_t;
typedef struct GwebAddTabBtnData gweb_add_tab_btn_data_t;
typedef struct GwebWebviewSettings gweb_webview_settings_t;
gweb_tabs_t *gweb_tabs_new();
GtkWidget *gweb_add_tab(GtkNotebook *notebook, gweb_tabs_t *tabs,
                        const char *uri, gweb_webview_settings_t *settings,
                        WebKitWebView *related);
void gweb_remove_tab(gweb_tabs_t *tabs, GtkNotebook *notebook, int page_num);
void gweb_tabs_destroy(gweb_tabs_t *tabs);
void gweb_add_tab_button_callback(GtkButton *button,
                                  gweb_add_tab_btn_data_t *data);

gweb_add_tab_btn_data_t *gweb_gen_data(GtkNotebook *notebook, gweb_tabs_t *tabs,
                                       gweb_webview_settings_t *settings,
                                       const char *uri);
void gweb_data_destroy(gweb_add_tab_btn_data_t *data);
gweb_webview_settings_t *gweb_settings_new(bool dev_tools, bool javascript,
                                           bool private);
void gweb_settings_destroy(gweb_webview_settings_t *settings);
#endif
