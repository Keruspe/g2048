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

#define SIZE 4

static gboolean
on_key (GtkWidget   *widget,
        GdkEventKey *event)
{
    G2048Grid *grid = G_2048_GRID (gtk_bin_get_child (GTK_BIN (widget)));

    switch (event->keyval)
    {
    case GDK_KEY_Up:
    case GDK_KEY_Down:
    case GDK_KEY_Left:
    case GDK_KEY_Right:
        if (g_2048_grid_on_key (grid, event->keyval))
        {
            printf ("You win\n");
            //exit (EXIT_SUCCESS);
        }
        break;
    default:
        return FALSE;
    };

    if (g_2048_grid_is_full (grid))
    {
        printf ("You lose\n");
        exit (EXIT_SUCCESS);
    }

    return FALSE;
}

gint
main (gint argc, gchar *argv[])
{
    gtk_init (&argc, &argv);
    g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);

    GtkApplication *app = gtk_application_new ("org.gnome.g2048", G_APPLICATION_FLAGS_NONE);
    GApplication *gapp = G_APPLICATION (app);
    GError *error = NULL;
    G_APPLICATION_GET_CLASS (gapp)->activate = NULL;

    g_application_register (gapp, NULL, &error);
    if (error)
    {
        fprintf (stderr, "Failed to register the gtk application: %s\n", error->message);
        g_error_free (error);
        return EXIT_FAILURE;
    }
    if (g_application_get_is_remote (gapp))
    {
        g_application_activate (gapp);
        return EXIT_SUCCESS;
    }

    GtkWidget *win = gtk_widget_new (GTK_TYPE_APPLICATION_WINDOW,
                                     "application",     app,
                                     "type",            GTK_WINDOW_TOPLEVEL,
                                     "window-position", GTK_WIN_POS_CENTER,
                                     "resizable",       FALSE,
                                     NULL);
    gtk_container_add (GTK_CONTAINER (win), g_2048_grid_new (SIZE));
    gtk_widget_show_all (win);
    GTK_WIDGET_GET_CLASS (win)->key_press_event = on_key;

    return g_application_run (gapp, argc, argv);
}
