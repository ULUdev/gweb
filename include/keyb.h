#ifndef __GWEB_KEYB_H__
#define __GWEB_KEYB_H__

#include "tabbing.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "stdbool.h"

typedef struct {
  gweb_tabs_t *tabs;
  GtkNotebook *notebook;
  gweb_webview_settings_t *settings;
  char *uri;
} gweb_shortcut_data_t;

bool gweb_handle_key_press(GtkWidget *widget, GdkEvent *event, gweb_shortcut_data_t *udata);

#endif
