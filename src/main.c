#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "tabbing.h"
#include "log.h"
#include "string.h"

#define GWEB_VERSION 1
#define GWEB_VERSION_STR "1.0.0"
#define streq(s1, s2) (gweb_streq(s1, s2) == 0)

int main(int argc, char **argv) {
	gweb_log_level verbosity = GWEB_LOG_ERR;
	bool dev_tools = true;
	bool javascript = true;
	for (int i=0;i<argc;i++) {
		if (streq(argv[i], "-v") || streq(argv[i], "--verbose")) {
			if (verbosity < GWEB_LOG_MSG) {
				verbosity += 1;
			}
		} else if (streq(argv[i], "-vv") || streq(argv[i], "--vverbose")) {
			if (verbosity < GWEB_LOG_WARN) {
				verbosity += 2;
			}
		} else if (streq(argv[i], "-V") || streq(argv[i], "--version")) {
			fprintf(stderr, "gweb version: %s\n", GWEB_VERSION_STR);
			exit(0);
		} else if (streq(argv[i], "--nojs")) {
			javascript = false;
		} else if (streq(argv[i], "--nodev")) {
			dev_tools = false;
		}
	}
	GtkWidget *window, *notebook;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	notebook = gtk_notebook_new();
	gweb_logger *logger = gweb_logger_new();
	gweb_set_log_level(logger, verbosity);

	gweb_log(logger, "Initalizing Gweb", GWEB_LOG_MSG);
	if (javascript) {
		gweb_log(logger, "javascript enabled", GWEB_LOG_MSG);
	} else {
		gweb_log(logger, "javascript disabled", GWEB_LOG_MSG);
	}
	if (dev_tools) {
		gweb_log(logger, "developer tools enabled", GWEB_LOG_MSG);
	} else {
		gweb_log(logger, "developer tools disabled", GWEB_LOG_MSG);
	}

	gweb_tabs_t *tabs = gweb_tabs_new(logger);
	gweb_webview_settings_t *websettings = gweb_settings_new(dev_tools, javascript);
	gweb_add_tab(GTK_NOTEBOOK(notebook), tabs, "about:blank", websettings);

	GtkWidget *add_tab = gtk_button_new_from_icon_name("add", GTK_ICON_SIZE_BUTTON);
	gweb_log(logger, "generating data for tab handling", GWEB_LOG_MSG);
	gweb_add_tab_btn_data_t *data = gweb_gen_data(GTK_NOTEBOOK(notebook), tabs, websettings);
	g_signal_connect(G_OBJECT(add_tab), "clicked", G_CALLBACK(gweb_add_tab_button_callback), data);
	gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), add_tab, GTK_PACK_END);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_container_add(GTK_CONTAINER(window), notebook);
	
	gtk_widget_show_all(window);
	gtk_widget_show(add_tab);
	gtk_main();
	gweb_log(logger, "freeing memory...", GWEB_LOG_MSG);
	gweb_logger_destroy(logger);
	gweb_tabs_destroy(tabs);
	gweb_data_destroy(data);
	gweb_settings_destroy(websettings);

	return 0;
}
