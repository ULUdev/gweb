#include "tabbing.h"
#include "log.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <assert.h>
#include "string.h"
#include <string.h>
#include <stdbool.h>

#define strstartswith(s1, s2) (gweb_strstartswith(s1, s2) == 0)

typedef struct GwebTab {
	void *data;
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
	GtkBox *box;
	WebKitWebView *webview;
};

struct GwebWebviewSettings {
	bool dev_tools;
	bool javascript;
};
struct GwebAddTabBtnData {
	GtkNotebook *notebook;
	gweb_tabs_t *tabs;
	gweb_webview_settings_t *settings;
};

typedef struct GwebLoadChangedUdata gweb_lc_udata;

void gweb_handle_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gweb_lc_udata *user_data) {
	assert(user_data != NULL);
	assert(user_data->notebook != NULL);
	assert(user_data->entry != NULL);
	assert(user_data->box != NULL);
	gtk_notebook_set_tab_label_text(user_data->notebook, GTK_WIDGET(user_data->box), webkit_web_view_get_title(web_view));
	gtk_entry_set_text(user_data->entry, webkit_web_view_get_uri(web_view));
}

gweb_tabs_t *gweb_tabs_new(gweb_logger *logger) {
	gweb_tabs_t *tabs = malloc(sizeof(gweb_tabs_t));
	tabs->head = NULL;
	tabs->count = 0;
	tabs->logger = logger;
	return tabs;
}

void gweb_entry_enter(GtkEntry *entry, gweb_lc_udata *user_data) {
	assert(user_data != NULL);
	assert(user_data->notebook != NULL);
	assert(user_data->entry != NULL);
	assert(user_data->box != NULL);
	assert(user_data->webview != NULL);
	char *uri = malloc(strlen(gtk_entry_get_text(entry)));
	strcpy(uri, gtk_entry_get_text(entry));
	if (!strstartswith(uri, "http://") && !strstartswith(uri, "https://")) {
		uri = realloc(uri, strlen(uri) + 7 + 1);
		char *tmp = malloc(strlen(uri) + 7 + 1);
		strcpy(tmp, "http://");
		uri = strcat(tmp, uri);
		// free(tmp);
	}
	webkit_web_view_load_uri(user_data->webview, uri);
	free(uri);
}

void gweb_add_tab(GtkNotebook *notebook, gweb_tabs_t *tabs, char *uri, gweb_webview_settings_t *settings) {
	GtkWidget *webview, *label, *box, *entry;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	webview = webkit_web_view_new();
	WebKitSettings *websettings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webview));
	g_object_set(G_OBJECT(websettings), "enable-developer-extras", settings->dev_tools, NULL);
	g_object_set(G_OBJECT(websettings), "enable-javascript", settings->javascript, NULL);

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(entry), false, false, 0);
	gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(webview), true, true, 0);
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), uri);
	label = gtk_label_new(webkit_web_view_get_title(WEBKIT_WEB_VIEW(webview)));
	gweb_lc_udata *data = malloc(sizeof(gweb_lc_udata));
	data->notebook = GTK_NOTEBOOK(notebook);
	data->entry = GTK_ENTRY(entry);
	data->box = GTK_BOX(box);
	data->webview = WEBKIT_WEB_VIEW(webview);

	gweb_tab_t *new = malloc(sizeof(gweb_tab_t));
	new->data = data;
	new->next = NULL;
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
	g_signal_connect(G_OBJECT(webview), "load-changed", G_CALLBACK(gweb_handle_load_changed), data);
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(gweb_entry_enter), data);
	int res = gtk_notebook_append_page(notebook, GTK_WIDGET(box), GTK_WIDGET(label));
	if (res == -1) {
		gweb_log(tabs->logger, "error creating tab", GWEB_LOG_ERR);
	} else {
		gweb_log(tabs->logger, "created tab", GWEB_LOG_MSG);
	}
	gtk_widget_show_all(box);
}

void gweb_remove_tab(gweb_tabs_t *tabs, GtkNotebook *notebook, int page_num) {
	if (tabs->count == 0) {
		return;
	}
	gweb_tab_t *prev = NULL;
	gweb_tab_t *cur = tabs->head;
	for (int i=0;i<page_num;i++) {
		if (cur == NULL) {
			break;
		} else {
			prev = cur;
			cur = cur->next;
		}
	}
	prev->next = cur->next;
	free(cur->data);
	free(cur);

	tabs->count--;
	gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_num);
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

void gweb_add_tab_button_callback(GtkButton *button, gweb_add_tab_btn_data_t *data) {
	assert(data != NULL);
	assert(data->notebook != NULL);
	assert(data->tabs != NULL);
	gweb_add_tab(data->notebook, data->tabs, "about:blank", data->settings);
}

gweb_add_tab_btn_data_t *gweb_gen_data(GtkNotebook *notebook, gweb_tabs_t *tabs, gweb_webview_settings_t *settings) {
	gweb_add_tab_btn_data_t *data = malloc(sizeof(gweb_add_tab_btn_data_t));
	data->notebook = notebook;
	data->tabs = tabs;
	data->settings = settings;
	return data;
}

void gweb_data_destroy(gweb_add_tab_btn_data_t *data) {
	free(data);
}
gweb_webview_settings_t *gweb_settings_new(bool dev_tools, bool javascript) {
	gweb_webview_settings_t *settings = malloc(sizeof(gweb_webview_settings_t));
	settings->dev_tools = dev_tools;
	settings->javascript = javascript;
	return settings;
}
void gweb_settings_destroy(gweb_webview_settings_t *settings) {
	free(settings);
}
