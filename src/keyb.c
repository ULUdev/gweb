#include "keyb.h"
#include "tabbing.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "stdbool.h"

bool gweb_handle_key_press(GtkWidget *widget, GdkEvent *event, gweb_shortcut_data_t *udata) {
  if (((GdkEventKey *)event)->state & GDK_CONTROL_MASK) {
    switch(((GdkEventKey *)event)->keyval) {
    case GDK_KEY_t:
      gweb_add_tab(udata->notebook, udata->tabs, udata->uri, udata->settings, NULL);
      return true;
    default:
      return false;
    }
  }
  return false;
}
