/*
 *      This file is part of g2048.
 *
 *      Copyright 2014 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
 *
 *      g2048 is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      g2048 is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with g2048.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "g2048-grid.h"

#include <stdlib.h>

#define DEFAULT_SIZE   4
#define DEFAULT_TARGET 2048
#define DEFAULT_THEME  "default"

__attribute__((noreturn)) static void
end (GtkWidget   *win,
     const gchar *msg)
{
    GTK_WIDGET_GET_CLASS (win)->key_press_event = NULL;
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (win),
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK,
                                                "%s", msg);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    exit (EXIT_SUCCESS);
}

static gboolean
on_key (GtkWidget   *widget,
        GdkEventKey *event)
{
    G2048Grid *grid = G_2048_GRID (gtk_container_get_children (GTK_CONTAINER (gtk_bin_get_child (GTK_BIN (widget))))->data);

    switch (event->keyval)
    {
    case GDK_KEY_Up:
    case GDK_KEY_Down:
    case GDK_KEY_Left:
    case GDK_KEY_Right:
        if (g_2048_grid_on_key (grid, event->keyval) && g_2048_grid_is_won (grid))
            end (widget, "You win");
        break;
    default:
        return FALSE;
    };

    if (g_2048_grid_is_lost (grid))
        end (widget, "You lose");

    return FALSE;
}

static void
show_win (GApplication *application)
{
    for (GList *wins = gtk_application_get_windows (GTK_APPLICATION (application)); wins; wins = g_list_next (wins))
        gtk_window_present (wins->data);
}

static gboolean
path_exists (const gchar *path)
{
    G_2048_CLEANUP_UNREF GFile *file = g_file_new_for_path (path);
    return g_file_query_exists (file, NULL);
}

static gchar *
get_theme_path (const gchar *theme)
{
    gchar *theme_path = g_build_filename (g_get_user_data_dir (), PACKAGE_NAME "/themes/", theme, NULL);
    if (path_exists (theme_path))
        return theme_path;
    g_free (theme_path);
    theme_path = g_build_filename (DATADIR "/" PACKAGE_NAME "/themes/", theme, NULL);
    if (path_exists (theme_path))
        return theme_path;
    g_free (theme_path);
    return NULL;
}

gint
main (gint argc, gchar *argv[])
{
    gtk_init (&argc, &argv);
    g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);

    gsize size = DEFAULT_SIZE;
    guint32 target = DEFAULT_TARGET;
    const gchar *theme = DEFAULT_THEME;

    GOptionEntry options[] = {
        { "size",   's',  0, G_OPTION_ARG_INT,    &size,   "The size of the grid", "4"           },
        { "target", '\0', 0, G_OPTION_ARG_INT,    &target, "The tile to reach",    "2048"        },
        { "theme",  't',  0, G_OPTION_ARG_STRING, &theme,  "The theme to use",     DEFAULT_THEME },
        { NULL,     '\0', 0, G_OPTION_ARG_NONE,   NULL,    NULL,                   NULL          }
    };
    G_2048_CLEANUP_OPTIONS_FREE GOptionContext *ctx = g_option_context_new ("foobar");
    g_option_context_add_main_entries (ctx, options, NULL);
    g_option_context_add_group (ctx, gtk_get_option_group (TRUE));
    g_option_context_parse (ctx, &argc, &argv, NULL);

    GtkApplication *app = gtk_application_new ("org.gnome.g2048", G_APPLICATION_FLAGS_NONE);
    GApplication *gapp = G_APPLICATION (app);
    G_2048_CLEANUP_ERROR_FREE GError *error = NULL;
    G_APPLICATION_GET_CLASS (gapp)->activate = show_win;

    g_application_register (gapp, NULL, &error);
    if (error)
    {
        fprintf (stderr, "Failed to register the gtk application: %s\n", error->message);
        return EXIT_FAILURE;
    }
    if (g_application_get_is_remote (gapp))
    {
        g_application_activate (gapp);
        return EXIT_SUCCESS;
    }

    G_2048_CLEANUP_FREE gchar *theme_path = get_theme_path (theme);
    if (!theme_path)
        theme_path = get_theme_path (DEFAULT_THEME);
    if (!theme_path)
    {
        g_critical ("No theme found");
        exit (EXIT_FAILURE);
    }

    GtkWidget *score_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    GtkBox *hbox = GTK_BOX (score_box);
    gtk_box_pack_start (hbox, gtk_label_new ("Score:"), TRUE, TRUE, 0);

    GtkWidget *score_label = gtk_label_new ("0");
    GtkLabel *label = GTK_LABEL (score_label);
    GdkRGBA color;
    gdk_rgba_parse (&color, "white");
    gtk_widget_override_background_color (score_label, GTK_STATE_FLAG_NORMAL, &color);
    gdk_rgba_parse (&color, "black");
    gtk_widget_override_color (score_label, GTK_STATE_FLAG_NORMAL, &color);
    gtk_label_set_width_chars (GTK_LABEL (score_label), 8);
    gtk_box_pack_end (hbox, score_label, TRUE, FALSE, 0);

    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    GtkBox *vbox = GTK_BOX (box);
    gtk_box_pack_start (vbox, g_2048_grid_new (size, target, theme_path, label), TRUE, TRUE, 0);
    gtk_box_pack_end (vbox, score_box, TRUE, TRUE, 20);

    GtkWidget *win = gtk_widget_new (GTK_TYPE_APPLICATION_WINDOW,
                                     "application",     app,
                                     "type",            GTK_WINDOW_TOPLEVEL,
                                     "window-position", GTK_WIN_POS_CENTER,
                                     "resizable",       FALSE,
                                     NULL);
    gtk_container_add (GTK_CONTAINER (win), box);
    gtk_widget_show_all (win);
    gtk_widget_override_font (win, pango_font_description_from_string("Monospace 18"));
    GTK_WIDGET_GET_CLASS (win)->key_press_event = on_key;

    return g_application_run (gapp, argc, argv);
}
