#include "tabbing.h"
#include "file.h"
#include "gweb_string.h"
#include "log.h"
#include "resourceblock.h"
#include <assert.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <webkit2/webkit2.h>

#define strstartswith(s1, s2) (gweb_strstartswith(s1, s2) == 0)
#define GWEB_NEW_TAB "New Tab"

// Structs: {{{
typedef struct GwebTab {
    void *data;
    GtkWidget *tab_widget;
    struct GwebTab *next;
} gweb_tab_t;

struct GwebTabs {
    gweb_tab_t *head;
    size_t count;
    gweb_logger *logger;
};

struct GwebLoadChangedUdata {
    GtkNotebook *notebook;
    GtkEntry *entry;
    GtkWidget *tab_widget;
    WebKitWebView *webview;
    GtkLabel *label;
    GtkProgressBar *pbar;
};

struct GwebWebviewSettings {
    bool dev_tools;
    bool javascript;
    bool private;
};
struct GwebAddTabBtnData {
    GtkNotebook *notebook;
    gweb_tabs_t *tabs;
    gweb_webview_settings_t *settings;
    char *uri;
};

struct TabRemoveData {
    GtkNotebook *notebook;
    GtkWidget *tab_widget;
    gweb_tabs_t *tabs;
};

typedef struct GwebLoadChangedUdata gweb_lc_udata;

// }}}

// Callbacks: {{{

void gweb_handle_wv_mouse_target_changed(WebKitWebView *web_view,
                                         WebKitHitTestResult *test_result,
                                         guint modifiers, GtkLabel *user_data) {
    assert(user_data != NULL);
    if (webkit_hit_test_result_context_is_link(test_result)) {
        char *uri =
            gweb_strdup(webkit_hit_test_result_get_link_uri(test_result));
        gweb_strabbrev(uri, 53);
        gtk_label_set_text(user_data, uri);
    } else {
        gtk_label_set_text(user_data, "");
    }
}

bool gweb_handle_se_changed(GtkSearchEntry *self, WebKitWebView *user_data) {
    WebKitFindController *fctl = webkit_web_view_get_find_controller(user_data);
    webkit_find_controller_search(fctl, gtk_entry_get_text(GTK_ENTRY(self)),
                                  WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE, 0);
    return true;
}

void gweb_handle_se_stop(GtkSearchEntry *self, WebKitWebView *user_data) {
    WebKitFindController *fctl = webkit_web_view_get_find_controller(user_data);
    webkit_find_controller_search_finish(fctl);
}

void gweb_handle_se_next(GtkSearchEntry *self, WebKitWebView *user_data) {
    webkit_find_controller_search_next(
        webkit_web_view_get_find_controller(user_data));
}
void gweb_handle_se_prev(GtkSearchEntry *self, WebKitWebView *user_data) {
    webkit_find_controller_search_previous(
        webkit_web_view_get_find_controller(user_data));
}

void gweb_webview_handle_terminated(WebKitWebView *web_view,
                                    WebKitWebProcessTerminationReason reason,
                                    gweb_tabs_t *tabs) {
    assert(tabs);
    assert(tabs->logger);
    gweb_logger *logger = tabs->logger;
    switch (reason) {
    case WEBKIT_WEB_PROCESS_CRASHED:
        gweb_log(logger, "web process has crashed", GWEB_LOG_ERR);
        break;
    case WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT:
        gweb_log(logger, "web process has exceeded its memory limit",
                 GWEB_LOG_ERR);
        break;
    case WEBKIT_WEB_PROCESS_TERMINATED_BY_API:
        gweb_log(logger, "web process was requested to terminate",
                 GWEB_LOG_MSG);
        break;
    }
}
void gweb_add_tab_button_callback(GtkButton *button,
                                  gweb_add_tab_btn_data_t *data) {
    assert(data != NULL);
    assert(data->notebook != NULL);
    assert(data->tabs != NULL);
    assert(data->uri != NULL);
    gweb_add_tab(data->notebook, data->tabs, data->uri, data->settings, NULL);
}
void gweb_entry_enter(GtkEntry *entry, gweb_lc_udata *user_data) {

    assert(user_data != NULL);
    assert(user_data->notebook != NULL);
    assert(user_data->entry != NULL);
    assert(user_data->tab_widget != NULL);
    assert(user_data->webview != NULL);

    char *uri = malloc(strlen(gtk_entry_get_text(entry)));
    strcpy(uri, gtk_entry_get_text(entry));

    if (!strstartswith(uri, "http://") && !strstartswith(uri, "https://") &&
        !strstartswith(uri, "file://")) {

        // dot in query. Probably a URL
        if (strchr(uri, '.')) {
            uri = realloc(uri, strlen(uri) + 7 + 1);
            char *tmp = malloc(strlen(uri) + 7 + 1);
            strcpy(tmp, "http://");
            uri = strcat(tmp, uri);
        } else {
            /*
             * treat as search
             * append the query to a search url (default
             * 'http://duckduckgo.com/?q=') so 'example' becomes
             * 'http://duckduckgo.com/?q=example'
             */
            char *query = malloc(strlen(uri) + strlen(GWEB_SEARCH_QUERY) + 1);
            strcpy(query, GWEB_SEARCH_QUERY);
            query = strcat(query, uri);
            free(uri);
            uri = query;
        }
    }

    webkit_web_view_load_uri(user_data->webview, uri);
    free(uri);
}

void gweb_back_callback(GtkButton *button, WebKitWebView *user_data) {
    assert(user_data != NULL);
    webkit_web_view_go_back(user_data);
}

void gweb_forward_callback(GtkButton *button, WebKitWebView *user_data) {
    assert(user_data != NULL);
    webkit_web_view_go_forward(user_data);
}

void gweb_reload_callback(GtkButton *button, WebKitWebView *user_data) {
    assert(user_data != NULL);
    webkit_web_view_reload(user_data);
}
GtkWidget *gweb_handle_webview_create(WebKitWebView *web_view,
                                      WebKitNavigationAction *nav_act,
                                      struct TabRemoveData *user_data) {
    assert(user_data);
    assert(user_data->notebook);
    assert(user_data->tabs);
    return gweb_add_tab(user_data->notebook, user_data->tabs,
                        webkit_uri_request_get_uri(
                            webkit_navigation_action_get_request(nav_act)),
                        NULL, web_view);
}

void gweb_remove_tab_callback(GtkButton *btn, struct TabRemoveData *user_data) {
    assert(user_data != NULL);
    assert(user_data->notebook != NULL);
    assert(user_data->tabs != NULL);
    assert(user_data->tab_widget != NULL);
    char *str = malloc(100);
    sprintf(str, "removing tab number %d",
            gtk_notebook_page_num(user_data->notebook, user_data->tab_widget));
    gweb_log(user_data->tabs->logger, str, GWEB_LOG_MSG);
    gweb_remove_tab(
        user_data->tabs, user_data->notebook,
        gtk_notebook_page_num(user_data->notebook, user_data->tab_widget));
    free(str);
    free(user_data);
}

void gweb_handle_load_changed(WebKitWebView *web_view,
                              WebKitLoadEvent load_event,
                              gweb_lc_udata *user_data) {
    assert(user_data != NULL);
    assert(user_data->notebook != NULL);
    assert(user_data->entry != NULL);
    assert(user_data->tab_widget != NULL);
    assert(user_data->label != NULL);
    assert(user_data->pbar != NULL);
    gtk_entry_set_text(user_data->entry, webkit_web_view_get_uri(web_view));

    gtk_widget_show(GTK_WIDGET(user_data->pbar));
    gtk_progress_bar_set_fraction(
        user_data->pbar, webkit_web_view_get_estimated_load_progress(web_view));
    if (load_event == WEBKIT_LOAD_FINISHED) {
        gtk_widget_hide(GTK_WIDGET(user_data->pbar));
        char *title = (char *)webkit_web_view_get_title(web_view);
        if (title == NULL) {
            title = malloc(strlen(GWEB_NEW_TAB) + 1);
            strcpy(title, GWEB_NEW_TAB);
        } else {
            title = malloc(strlen(title) + 1);
            strcpy(title, webkit_web_view_get_title(web_view));
        }
        if (strlen(title) > 23) {
            char *new_title = malloc(24);
            for (int i = 0; i < 20; i++) {
                new_title[i] = title[i];
            }
            free(title);
            // add three dots and a null terminator
            new_title[19] = '.';
            new_title[20] = '.';
            new_title[21] = '.';
            new_title[22] = '\0';
            title = new_title;
        }
        gtk_label_set_label(user_data->label, title);
        // gtk_label_set_label(user_data->label,
        // webkit_web_view_get_title(web_view));
        free(title);
    }
}

// }}}

// Tab related functions: {{{
gweb_tabs_t *gweb_tabs_new(gweb_logger *logger) {
    gweb_tabs_t *tabs = malloc(sizeof(gweb_tabs_t));
    tabs->head = NULL;
    tabs->count = 0;
    tabs->logger = logger;
    return tabs;
}

void gweb_remove_tab(gweb_tabs_t *tabs, GtkNotebook *notebook, int page_num) {
    if (tabs->count == 0) {
        return;
    } else if (tabs->count == 1) {
        gtk_main_quit();
        return;
    }
    GtkWidget *tab_widget = gtk_notebook_get_nth_page(notebook, page_num);
    if (tab_widget == NULL) {
        // tab number out of bounds
        return;
    }
    gweb_tab_t *prev = NULL;
    gweb_tab_t *cur = tabs->head;
    while (cur != NULL && cur->tab_widget != tab_widget) {
        prev = cur;
        cur = cur->next;
    }
    // attempting to remove the first page would cause a segfault normally if
    // you try to set the previos next pointer to the current next pointer
    if (prev == NULL) {
        tabs->head = tabs->head->next;
    } else if (cur == NULL) {
        return;
    } else {
        prev->next = cur->next;
    }
    free(cur->data);
    free(cur);

    tabs->count--;
    gtk_notebook_remove_page(notebook, page_num);
}

void gweb_tabs_destroy(gweb_tabs_t *tabs) {
    gweb_tab_t *current = tabs->head;
    gweb_log(tabs->logger, "cleaning up tabs", GWEB_LOG_MSG);
    while (current != NULL) {
        gweb_tab_t *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    free(tabs);
}

// create a new tab from a related webview or from gweb_webview_settings_t
GtkWidget *gweb_add_tab(GtkNotebook *notebook, gweb_tabs_t *tabs,
                        const char *uri, gweb_webview_settings_t *settings,
                        WebKitWebView *related) {
    GtkWidget *webview, *label, *hbox, *hbox2, *vbox, *entry, *tab_box,
        *tab_closebtn, *tab_forward, *tab_back, *tab_reload, *load_pbar,
        *search_entry, *hoverlabel;
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, GWEB_BOX_SPACING);
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, GWEB_BOX_SPACING);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, GWEB_BOX_SPACING);

    tab_back = gtk_button_new_from_icon_name("go-previous-symbolic",
                                             GTK_ICON_SIZE_BUTTON);
    tab_forward =
        gtk_button_new_from_icon_name("go-next-symbolic", GTK_ICON_SIZE_BUTTON);
    tab_reload = gtk_button_new_from_icon_name("reload", GTK_ICON_SIZE_BUTTON);
    entry = gtk_entry_new();
    load_pbar = gtk_progress_bar_new();
    search_entry = gtk_search_entry_new();
    hoverlabel = gtk_label_new("");

    if (related) {
        webview = webkit_web_view_new_with_related_view(related);
    } else if (settings) {
        webview = webkit_web_view_new();
        WebKitSettings *websettings =
            webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webview));
        webkit_settings_set_enable_javascript(websettings,
                                              settings->javascript);
        webkit_settings_set_enable_developer_extras(websettings,
                                                    settings->dev_tools);
        webkit_settings_set_enable_page_cache(websettings, true);

        webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webview), websettings);
    } else {
        gweb_log(tabs->logger, "cannot create webview", GWEB_LOG_ERR);

        // some memory issues
        abort();
    }
    gweb_load_filter_list("filters.json", WEBKIT_WEB_VIEW(webview),
                          tabs->logger);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(load_pbar), false);

    g_signal_connect(G_OBJECT(tab_back), "clicked",
                     G_CALLBACK(gweb_back_callback), webview);
    g_signal_connect(G_OBJECT(tab_forward), "clicked",
                     G_CALLBACK(gweb_forward_callback), webview);
    g_signal_connect(G_OBJECT(tab_reload), "clicked",
                     G_CALLBACK(gweb_reload_callback), webview);
    g_signal_connect(G_OBJECT(search_entry), "search-changed",
                     G_CALLBACK(gweb_handle_se_changed), webview);
    g_signal_connect(G_OBJECT(search_entry), "stop-search",
                     G_CALLBACK(gweb_handle_se_stop), webview);
    g_signal_connect(G_OBJECT(search_entry), "next-match",
                     G_CALLBACK(gweb_handle_se_next), webview);
    g_signal_connect(G_OBJECT(search_entry), "previous-match",
                     G_CALLBACK(gweb_handle_se_prev), webview);

    g_signal_connect(G_OBJECT(webview), "mouse-target-changed",
                     G_CALLBACK(gweb_handle_wv_mouse_target_changed),
                     hoverlabel);

    gtk_widget_set_tooltip_text(tab_back, "Back");
    gtk_widget_set_tooltip_text(tab_forward, "Forward");
    gtk_widget_set_tooltip_text(tab_reload, "Reload page");

    gtk_box_pack_start(GTK_BOX(hbox), tab_back, false, false, 1);
    gtk_box_pack_start(GTK_BOX(hbox), tab_forward, false, false, 1);
    gtk_box_pack_start(GTK_BOX(hbox), tab_reload, false, false, 1);
    gtk_box_pack_end(GTK_BOX(hbox), entry, true, true, 1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 1);
    gtk_box_pack_start(GTK_BOX(vbox), load_pbar, false, false, 1);
    gtk_box_pack_start(GTK_BOX(vbox), webview, true, true, 1);
    gtk_box_pack_end(GTK_BOX(vbox), hbox2, false, false, 1);
    gtk_box_pack_start(GTK_BOX(hbox2), search_entry, true, true, 1);
    gtk_box_pack_end(GTK_BOX(hbox2), hoverlabel, true, true, 1);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), uri);

    label = gtk_label_new(GWEB_NEW_TAB);
    tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    tab_closebtn =
        gtk_button_new_from_icon_name("tab-close", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(tab_box), GTK_WIDGET(label), true, true, 0);
    gtk_box_pack_end(GTK_BOX(tab_box), GTK_WIDGET(tab_closebtn), false, false,
                     0);

    gtk_widget_set_tooltip_text(tab_closebtn, "Close this tab");

    gweb_lc_udata *data = malloc(sizeof(gweb_lc_udata));
    data->notebook = GTK_NOTEBOOK(notebook);
    data->entry = GTK_ENTRY(entry);
    data->tab_widget = vbox;
    data->webview = WEBKIT_WEB_VIEW(webview);
    data->label = GTK_LABEL(label);
    data->pbar = GTK_PROGRESS_BAR(load_pbar);

    gweb_tab_t *new = malloc(sizeof(gweb_tab_t));
    new->data = data;
    new->next = NULL;
    new->tab_widget = vbox;

    if (tabs->head == NULL) {
        tabs->head = new;
    } else {
        gweb_tab_t *current = tabs->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new;
    }

    tabs->count++;

    if (settings && !settings->private) {
        WebKitWebContext *ctx =
            webkit_web_view_get_context(WEBKIT_WEB_VIEW(webview));
        WebKitCookieManager *cookie_man =
            webkit_web_context_get_cookie_manager(ctx);
        char *cookiefile = gweb_cookie_file();
        webkit_cookie_manager_set_persistent_storage(
            cookie_man, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
        free(cookiefile);
    }

    g_signal_connect(G_OBJECT(webview), "load-changed",
                     G_CALLBACK(gweb_handle_load_changed), data);
    g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(gweb_entry_enter),
                     data);
    int res = gtk_notebook_append_page(notebook, GTK_WIDGET(vbox),
                                       GTK_WIDGET(tab_box));
    if (res == -1) {
        gweb_log(tabs->logger, "error creating tab", GWEB_LOG_ERR);
        return NULL;
    } else {

        // get the exact size required
        char *str = malloc(100);
        sprintf(str, "created tab with id %d", res);
        gweb_log(tabs->logger, str, GWEB_LOG_MSG);
        free(str);
        gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(vbox), true);
    }
    struct TabRemoveData *tab_remove_data =
        malloc(sizeof(struct TabRemoveData));
    tab_remove_data->notebook = GTK_NOTEBOOK(notebook);
    tab_remove_data->tabs = tabs;
    // tab_remove_data->tabid = res;
    tab_remove_data->tab_widget = vbox;
    g_signal_connect(G_OBJECT(tab_closebtn), "clicked",
                     G_CALLBACK(gweb_remove_tab_callback), tab_remove_data);
    g_signal_connect(G_OBJECT(webview), "create",
                     G_CALLBACK(gweb_handle_webview_create), tab_remove_data);
    gtk_widget_show_all(vbox);
    gtk_widget_show_all(hbox);
    gtk_widget_show_all(hbox2);
    gtk_widget_show_all(tab_box);

    gtk_notebook_set_current_page(notebook, res);
    gtk_widget_grab_focus(GTK_WIDGET(entry));
    return webview;
}

// }}}

// Tab data: {{{
gweb_add_tab_btn_data_t *gweb_gen_data(GtkNotebook *notebook, gweb_tabs_t *tabs,
                                       gweb_webview_settings_t *settings,
                                       const char *uri) {
    gweb_add_tab_btn_data_t *data = malloc(sizeof(gweb_add_tab_btn_data_t));
    data->notebook = notebook;
    data->tabs = tabs;
    data->settings = settings;
    data->uri = gweb_strdup(uri);
    return data;
}

void gweb_data_destroy(gweb_add_tab_btn_data_t *data) {
    free(data->uri);
    free(data);
}

gweb_webview_settings_t *gweb_settings_new(bool dev_tools, bool javascript,
                                           bool private) {
    gweb_webview_settings_t *settings = malloc(sizeof(gweb_webview_settings_t));
    settings->dev_tools = dev_tools;
    settings->javascript = javascript;
    settings->private = private;
    return settings;
}

void gweb_settings_destroy(gweb_webview_settings_t *settings) {
    free(settings);
}
// }}}

// vim: fdm=marker
