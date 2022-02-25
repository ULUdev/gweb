#include "resourceblock.h"
#include <stdbool.h>
#include <webkit2/webkit2.h>
#include "file.h"
#include "log.h"
#include <stdio.h>

struct GwebHandleFinishData {
	WebKitUserContentManager *cman;
	gweb_logger *logger;
};

static void gweb_handle_ready(WebKitUserContentFilterStore *store, GAsyncResult *result, gpointer user_data) {
	struct GwebHandleFinishData *finish_data = (struct GwebHandleFinishData *)user_data;
	GError *err = NULL;
	WebKitUserContentFilter *filter = webkit_user_content_filter_store_load_finish(store, result, &err);
	if (filter == NULL) {
		gweb_log(finish_data->logger, "failed to load filter file", GWEB_LOG_WARN);
	} else {
		webkit_user_content_manager_add_filter(WEBKIT_USER_CONTENT_MANAGER(finish_data->cman), filter);
		gweb_log(finish_data->logger, "adding filter to content manager", GWEB_LOG_MSG);
	}
	free(user_data);
}

static void gweb_handle_finish(WebKitUserContentFilterStore *store, GAsyncResult *result, gpointer user_data) {
	GError *error = NULL;
	struct GwebHandleFinishData *finish_data = (struct GwebHandleFinishData *)user_data;
	WebKitUserContentFilter *filter = webkit_user_content_filter_store_save_from_file_finish(store, result, &error);
	if (filter == NULL) {
		if (error != NULL) {
			char *msg;
			sprintf(msg, "failed to compile filter file [%d]: %s", error->code, error->message);
			gweb_log(finish_data->logger, msg, GWEB_LOG_WARN);
		} else {
			gweb_log(finish_data->logger, "failed to compile filter file", GWEB_LOG_WARN);
		}
	} else {
		webkit_user_content_filter_store_load(store, "a", NULL, (GAsyncReadyCallback)gweb_handle_ready, finish_data);
		gweb_log(finish_data->logger, "adding filter to content manager", GWEB_LOG_MSG);
	}

}

/*
 * load a filter list from file_name into the user_content_manager of web_view
 */
void gweb_load_filter_list(const char *file_name, WebKitWebView *web_view, gweb_logger *logger) {
	WebKitUserContentManager *cman = webkit_web_view_get_user_content_manager(web_view);
	struct GwebHandleFinishData *udata = malloc(sizeof(struct GwebHandleFinishData));
	udata->cman = cman;
	udata->logger = logger;

	WebKitUserContentFilterStore *store = webkit_user_content_filter_store_new(g_build_filename(g_get_user_data_dir(), "gweb", "filters", NULL));
	GFile *filter_file = g_file_new_for_path(g_build_filename(g_get_user_config_dir(), "gweb", "filters.json", NULL));
	if (filter_file == NULL) {
		gweb_log(logger, "opening filter file failed", GWEB_LOG_WARN);
		return;
	}
	webkit_user_content_filter_store_save_from_file(store, "a", filter_file, NULL, (GAsyncReadyCallback)gweb_handle_finish, udata);
}
