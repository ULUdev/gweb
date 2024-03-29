#include "config.h"
#include "gweb_string.h"
#include "hashmap.h"
#include "keyb.h"
#include "linked_list.h"
#include "log.h"
#include "tabbing.h"
#include "url.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <webkit2/webkit2.h>
#ifdef _WIN32
#error "Windows is not supported"
#endif

// library version
#define GWEB_VERSION_MAJOR 1
#define GWEB_VERSION_MINOR 6
#define GWEB_VERSION_PATCH 7

#define streq(s1, s2) (gweb_streq(s1, s2) == 0)

extern int gweb_log_level_v;

const char *GWEB_HELP_STR =
    "\n"
    "SYNOPSIS\n"
    "  gweb -[Vvh]\n\n"
    "OPTIONS\n"
    "  -h,--help: print this help and exit\n"
    "  -V,--version: print the version and exit\n"
    "  -v,--verbose: verbose logging\n"
    "  -vv,--vverbose: more verbose logging\n"
    "  --nojs: disable javascript\n"
    "  --nodev: disable developer tools\n"
    "  --noconf: disable configuration reading\n"
    "  --headless: launch a minimal gui version that uses the last url "
    "supplied as a url to be rendered\n"
    "  --private: enable private browsing\n"
    "\n";

int main(int argc, char **argv) {
    int verbosity = 0;
    bool dev_tools = true;
    bool javascript = true;
    bool use_config = true;
    bool private = false;
    bool headless = false;
    linked_list_t *urls = linked_list_new();
    for (int i = 1; i < argc; i++) {
        if (streq(argv[i], "-v") || streq(argv[i], "--verbose")) {
            if (verbosity < 2) {
                verbosity += 1;
            }
        } else if (streq(argv[i], "-vv") || streq(argv[i], "--vverbose")) {
            if (verbosity < 1) {
                verbosity += 2;
            }
        } else if (streq(argv[i], "-V") || streq(argv[i], "--version")) {
            fprintf(stderr, "gweb version: %d.%d.%d\n", GWEB_VERSION_MAJOR,
                    GWEB_VERSION_MINOR, GWEB_VERSION_PATCH);
            exit(0);
        } else if (streq(argv[i], "--nojs")) {
            javascript = false;
        } else if (streq(argv[i], "--nodev")) {
            dev_tools = false;
        } else if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            fprintf(stderr, "%s", GWEB_HELP_STR);
            exit(0);
        } else if (streq(argv[i], "--noconf")) {
            use_config = false;
        } else if (streq(argv[i], "--headless")) {
            headless = true;
        } else if (streq(argv[i], "--private")) {
            private = true;
        } else {
            linked_list_push(urls, argv[i]);
        }
    }
    GtkWidget *window, *notebook;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 900);
    notebook = gtk_notebook_new();
    gweb_log_set(&gweb_log_level_v, verbosity);

    gweb_log_msg("Initalizing gweb v%d.%d.%d", GWEB_VERSION_MAJOR,
                 GWEB_VERSION_MINOR, GWEB_VERSION_PATCH);

    if (headless) {
        if (linked_list_size(urls) > 0) {
            GtkWidget *hlwv = webkit_web_view_new();
            webkit_web_view_load_uri(WEBKIT_WEB_VIEW(hlwv),
                                     (char *)linked_list_pop(urls));
            g_signal_connect(G_OBJECT(window), "destroy",
                             G_CALLBACK(gtk_main_quit), NULL);
            gtk_container_add(GTK_CONTAINER(window), hlwv);
            gtk_widget_show_all(window);
            gtk_main();
            gweb_log_msg("exitting headless browser process");
            return 0;
        } else {
            gweb_log_msg("no url found for headless mode");
        }
    }

    hashmap_t *config = NULL;
    if (use_config) {
        config = gweb_parse_config(
            g_build_filename(g_get_user_config_dir(), "gweb", "config", NULL));

        char *confstr = hashmap_read(config, "javascript");
        if (confstr != NULL) {
            int res = gweb_parse_bool_str(confstr);
            if (res == 0) {
                javascript = true;
            } else if (res == 1) {
                javascript = false;
            } else {
                gweb_log_err(
                    "configuration attribute 'javascript' has invalid value");
            }
        }
    }

    gweb_log_msg("javascript: %s", javascript ? "enabled" : "disabled");
    gweb_log_msg("developer tools: %s", dev_tools ? "enabled" : "disabled");
    gweb_log_msg("private browsing: %s", private ? "enabled" : "disabled");

    gweb_tabs_t *tabs = gweb_tabs_new();
    gweb_webview_settings_t *websettings =
        gweb_settings_new(dev_tools, javascript, private);
    char *new_tab_url = "about:blank";
    if (use_config) {
        char *tmp = hashmap_read(config, "new_tab_url");
        if (tmp != NULL) {
            new_tab_url = tmp;
        }
    }

    gweb_shortcut_data_t *shdata = malloc(sizeof(gweb_shortcut_data_t));
    shdata->tabs = tabs;
    shdata->notebook = GTK_NOTEBOOK(notebook);
    shdata->uri = new_tab_url;
    shdata->settings = websettings;

    g_signal_connect(G_OBJECT(window), "key-press-event",
                     G_CALLBACK(gweb_handle_key_press), shdata);

    if (linked_list_size(urls) == 0) {
        gweb_add_tab(GTK_NOTEBOOK(notebook), tabs, new_tab_url, websettings,
                     NULL);
    }
    while (linked_list_size(urls) != 0) {
        char *url = linked_list_pop(urls);
        char *processed_url = process_url(url);
        gweb_add_tab(GTK_NOTEBOOK(notebook), tabs, processed_url, websettings,
                     NULL);
        if (url != processed_url)
            free(processed_url);
    }
    linked_list_destroy(urls);
    GtkWidget *add_tab =
        gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_tooltip_text(add_tab, "Add a new tab");
    gweb_log_msg("generating data for tab handling");
    gweb_add_tab_btn_data_t *data =
        gweb_gen_data(GTK_NOTEBOOK(notebook), tabs, websettings, new_tab_url);
    g_signal_connect(G_OBJECT(add_tab), "clicked",
                     G_CALLBACK(gweb_add_tab_button_callback), data);
    gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), add_tab,
                                   GTK_PACK_END);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
                     NULL);
    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_widget_show_all(window);
    gtk_widget_show(add_tab);
    gtk_main();
    gweb_log_msg("freeing memory...");
    gweb_tabs_destroy(tabs);
    gweb_data_destroy(data);
    gweb_settings_destroy(websettings);
    free(shdata);
    hashmap_destroy(config);

    return 0;
}
