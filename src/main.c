#include "gweb_string.h"
#include "linked_list/linked_list.h"
#include "log.h"
#include "tabbing.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// library version
#define GWEB_VERSION 1

// version string for commandline and packaging purposes
#define GWEB_VERSION_STR "1.0.3"
#define streq(s1, s2) (gweb_streq(s1, s2) == 0)

const char *GWEB_HELP_STR = "\n"
                            "SYNOPSIS\n"
                            "  gweb -[Vvh]\n\n"
                            "OPTIONS\n"
                            "  -h,--help: print this help and exit\n"
                            "  -V,--version: print the version and exit\n"
                            "  -v,--verbose: verbose logging\n"
                            "  -vv,--vverbose: more verbose logging\n"
                            "  --nojs: disable javascript\n"
                            "  --nodev: disable developer tools\n\n";

int main(int argc, char **argv) {
    gweb_log_level verbosity = GWEB_LOG_ERR;
    bool dev_tools = true;
    bool javascript = true;
    linked_list_t *urls = linked_list_new();
    for (int i = 1; i < argc; i++) {
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
        } else if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            fprintf(stderr, GWEB_HELP_STR);
            exit(0);
        } else {
            linked_list_push(urls, argv[i]);
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
    gweb_webview_settings_t *websettings =
        gweb_settings_new(dev_tools, javascript);
    if (linked_list_size(urls) == 0) {
        gweb_add_tab(GTK_NOTEBOOK(notebook), tabs, "about:blank", websettings);
    }
    while (linked_list_size(urls) != 0) {
        char *url = linked_list_pop(urls);
        gweb_add_tab(GTK_NOTEBOOK(notebook), tabs, url, websettings);
    }
    linked_list_destroy(urls);
    GtkWidget *add_tab =
        gtk_button_new_from_icon_name("add", GTK_ICON_SIZE_BUTTON);
    gweb_log(logger, "generating data for tab handling", GWEB_LOG_MSG);
    gweb_add_tab_btn_data_t *data =
        gweb_gen_data(GTK_NOTEBOOK(notebook), tabs, websettings);
    g_signal_connect(G_OBJECT(add_tab), "clicked",
                     G_CALLBACK(gweb_add_tab_button_callback), data);
    gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), add_tab,
                                   GTK_PACK_END);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
                     NULL);
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // GtkAccelGroup *accels = gtk_accel_group_new();
    // gtk_accel_group_connect(accels, gdk_keyval_from_name("t"),
    // GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, G_CLOSURE())
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
