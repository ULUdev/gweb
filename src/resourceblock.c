#include "resourceblock.h"
#include "file.h"
#include "log.h"
#include <stdbool.h>
#include <stdio.h>
#include <webkit2/webkit2.h>

typedef WebKitUserContentManager gweb_handle_finish_data_t;

static void gweb_handle_ready(WebKitUserContentFilterStore *store,
                              GAsyncResult *result, gpointer user_data) {
    gweb_handle_finish_data_t *finish_data =
        (gweb_handle_finish_data_t *)user_data;
    GError *err = NULL;
    WebKitUserContentFilter *filter =
        webkit_user_content_filter_store_load_finish(store, result, &err);
    if (filter == NULL) {
        gweb_log_wrn("failed to load filter file");
    } else {
        webkit_user_content_manager_add_filter(
            WEBKIT_USER_CONTENT_MANAGER(finish_data), filter);
        gweb_log_msg("adding filter to content manager");
    }
}

static void gweb_handle_finish(WebKitUserContentFilterStore *store,
                               GAsyncResult *result, gpointer user_data) {
    GError *error = NULL;
    gweb_handle_finish_data_t *finish_data =
        (gweb_handle_finish_data_t *)user_data;
    WebKitUserContentFilter *filter =
        webkit_user_content_filter_store_save_from_file_finish(store, result,
                                                               &error);
    if (filter == NULL) {
        if (error != NULL) {
            gweb_log_wrn("failed to compile filter file [%d]: %s", error->code,
                         error->message);
        } else {
            gweb_log_wrn("failed to compile filter file");
        }
    } else {
        webkit_user_content_filter_store_load(
            store, "a", NULL, (GAsyncReadyCallback)gweb_handle_ready,
            finish_data);
        gweb_log_msg("adding filter to content manager");
    }
}

/*
 * load a filter list from file_name into the user_content_manager of web_view
 */
void gweb_load_filter_list(const char *file_name, WebKitWebView *web_view) {
    WebKitUserContentManager *cman =
        webkit_web_view_get_user_content_manager(web_view);

    WebKitUserContentFilterStore *store = webkit_user_content_filter_store_new(
        g_build_filename(g_get_user_data_dir(), "gweb", "filters", NULL));
    const char *path =
        g_build_filename(g_get_user_config_dir(), "gweb", "filters.json", NULL);
    if (!gweb_fexists(path)) {
        gweb_log_wrn("filter file doesn't exist");
        return;
    }
    GFile *filter_file = g_file_new_for_path(path);
    if (filter_file == NULL) {
        gweb_log_wrn("opening filter file failed");
        return;
    }
    webkit_user_content_filter_store_save_from_file(
        store, "a", filter_file, NULL, (GAsyncReadyCallback)gweb_handle_finish,
        cman);
}
