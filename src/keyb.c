#include "keyb.h"
#include "stdbool.h"
#include "tabbing.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

bool gweb_handle_key_press(GtkWidget *widget, GdkEvent *event,
                           gweb_shortcut_data_t *udata) {
    GdkEventKey *key = (GdkEventKey *)event;
    if (key->state & GDK_CONTROL_MASK) {
        switch (key->keyval) {
        case GDK_KEY_t:
            gweb_add_tab(udata->notebook, udata->tabs, udata->uri,
                         udata->settings, NULL);
            return true;
        case GDK_KEY_w:
            gweb_remove_tab(udata->tabs, udata->notebook,
                            gtk_notebook_get_current_page(udata->notebook));
        default:
            return false;
        }
    }
    return false;
}
