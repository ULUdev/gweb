#include "tabbing.h"
#include "log.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <assert.h>
#include "string.h"
#include <string.h>
#include <stdbool.h>
#include "file.h"

#define strstartswith(s1, s2) (gweb_strstartswith(s1, s2) == 0)
#define GWEB_NEW_TAB "New Tab"

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
	GtkLabel *label;
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

struct TabRemoveData {
	GtkNotebook *notebook;
	gweb_tabs_t *tabs;
	int tabid;
};

void gweb_remove_tab_callback(GtkButton *btn, struct TabRemoveData *user_data) {
	assert(user_data != NULL);
	assert(user_data->notebook != NULL);
	assert(user_data->tabs != NULL);
	assert(user_data->tabid != -1);
	char str[100];
	sprintf(str, "removing tab with id %d", user_data->tabid);
	gweb_log(user_data->tabs->logger, str, GWEB_LOG_MSG);
	gweb_remove_tab(user_data->tabs, user_data->notebook, user_data->tabid);
	free(user_data);
}

typedef struct GwebLoadChangedUdata gweb_lc_udata;

void gweb_handle_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gweb_lc_udata *user_data) {
	assert(user_data != NULL);
	assert(user_data->notebook != NULL);
	assert(user_data->entry != NULL);
	assert(user_data->box != NULL);
	assert(user_data->label != NULL);
	gtk_entry_set_text(user_data->entry, webkit_web_view_get_uri(web_view));

	if (load_event == WEBKIT_LOAD_FINISHED) {
		char *title = webkit_web_view_get_title(web_view);
		if (title == NULL) {
			title = malloc(strlen(GWEB_NEW_TAB) +1);
			strcpy(title, GWEB_NEW_TAB);
		} else {
			title = malloc(strlen(title) + 1);
			char *tmp = webkit_web_view_get_title(web_view);
			strcpy(title, tmp);
		}
		if (strlen(title) > 23) {
			char *new_title = malloc(24);
			for (int i=0;i<20;i++) {
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
		// gtk_label_set_label(user_data->label, webkit_web_view_get_title(web_view));
		free(title);
	}
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
	GtkWidget *webview, *label, *box, *entry, *tab_box, *tab_closebtn;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	webview = webkit_web_view_new();
	WebKitSettings *websettings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webview));
	g_object_set(G_OBJECT(websettings), "enable-developer-extras", settings->dev_tools, NULL);
	g_object_set(G_OBJECT(websettings), "enable-javascript", settings->javascript, NULL);

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(entry), false, false, 0);
	gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(webview), true, true, 0);
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), uri);
	label = gtk_label_new(GWEB_NEW_TAB);

	tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	tab_closebtn = gtk_button_new_from_icon_name("tab-close", GTK_ICON_SIZE_BUTTON);
	gtk_box_pack_start(GTK_BOX(tab_box), GTK_WIDGET(label), true, true, 0);
	gtk_box_pack_end(GTK_BOX(tab_box), GTK_WIDGET(tab_closebtn), false, false, 0);

	gweb_lc_udata *data = malloc(sizeof(gweb_lc_udata));
	data->notebook = GTK_NOTEBOOK(notebook);
	data->entry = GTK_ENTRY(entry);
	data->box = GTK_BOX(box);
	data->webview = WEBKIT_WEB_VIEW(webview);
	data->label = label;

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
	
	// WebKitWebContext *ctx = webkit_web_view_get_context(WEBKIT_WEB_VIEW(webview));
	// WebKitCookieManager *cookie_man = webkit_web_context_get_cookie_manager(ctx);
	// char *cookiefile = gweb_cookie_file();
	// webkit_cookie_manager_set_persistent_storage(cookie_man, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
	// free(cookiefile);

	g_signal_connect(G_OBJECT(webview), "load-changed", G_CALLBACK(gweb_handle_load_changed), data);
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(gweb_entry_enter), data);
	int res = gtk_notebook_append_page(notebook, GTK_WIDGET(box), GTK_WIDGET(tab_box));
	if (res == -1) {
		gweb_log(tabs->logger, "error creating tab", GWEB_LOG_ERR);
	} else {
		char str[100];
		sprintf(str, "created tab with id %d", res);
		gweb_log(tabs->logger, str, GWEB_LOG_MSG);
	}
	struct TabRemoveData *tab_remove_data = malloc(sizeof(struct TabRemoveData));
	tab_remove_data->notebook = GTK_NOTEBOOK(notebook);
	tab_remove_data->tabs = tabs;
	tab_remove_data->tabid = res;
	g_signal_connect(G_OBJECT(tab_closebtn), "clicked", G_CALLBACK(gweb_remove_tab_callback), tab_remove_data);
	gtk_widget_show_all(box);
	gtk_widget_show_all(tab_box);
}

void gweb_remove_tab(gweb_tabs_t *tabs, GtkNotebook *notebook, int page_num) {
	if (tabs->count == 0) {
		return;
	} else if (tabs->count == 1) {
		gtk_main_quit();
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

void gweb_add_tab_button_callback(GtkButton *button, gweb_add_tab_btn_data_t *data) {
	assert(data != NULL);
	assert(data->notebook != NULL);
	assert(data->tabs != -1);
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
