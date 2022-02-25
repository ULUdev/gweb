#ifndef __GWEB_RESOURCE_BLOCK_H__
#define __GWEB_RESOURCE_BLOCK_H__
/*
 * This file is for blocking resources using webkits user_content_filter_store
 * is added to the user_content_manager
 */
#include <webkit2/webkit2.h>
#include "log.h"

static void gweb_handle_finish(WebKitUserContentFilterStore *store, GAsyncResult *result, gpointer user_data);
void gweb_load_filter_list(const char *file_name, WebKitWebView *web_view, gweb_logger *logger);
#endif
