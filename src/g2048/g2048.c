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

#include <gtk/gtk.h>

#include <stdlib.h>

static GtkWidget *
_gtk_label_new (void)
{
    GtkWidget *label = gtk_widget_new (GTK_TYPE_LABEL,
                                       "label",          "2048",
                                       "xalign",         0.5,
                                       "yalign",         0.5,
                                       "height-request", 100,
                                       "width-request",  100,
                                       NULL);
    GdkRGBA color;
    gdk_rgba_parse (&color, "blue");
    gtk_widget_override_background_color (label, GTK_STATE_FLAG_NORMAL, &color);
    return label;
}

static void
_gtk_grid_add_row (GtkGrid *grid,
                   guint    row_nb,
                   gsize    size)
{
    GtkWidget *label, *previous_label;
    gtk_grid_attach (grid, (label = _gtk_label_new ()), 0, row_nb, 1, 1);
    for (gsize s = 1; s < size; ++s)
    {
        previous_label = label;
        gtk_grid_attach_next_to (grid, (label = _gtk_label_new ()), previous_label, GTK_POS_RIGHT, 1, 1);
    }
}


static void
_gtk_grid_init (GtkGrid *grid,
                gsize    size)
{
    for (gsize s = 0; s < size; ++s)
        _gtk_grid_add_row (grid, s, size);
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

    GtkWidget *grid = gtk_widget_new (GTK_TYPE_GRID,
                                      "column-homogeneous", TRUE,
                                      "row-homogeneous",    TRUE,
                                      "column-spacing",     10,
                                      "row-spacing",        10,
                                      "border-width",       15,
                                      NULL);
    GtkGrid *g = GTK_GRID (grid);
    _gtk_grid_init (g, 4);

    GtkWidget *win = gtk_widget_new (GTK_TYPE_APPLICATION_WINDOW,
                                     "application",     app,
                                     "type",            GTK_WINDOW_TOPLEVEL,
                                     "window-position", GTK_WIN_POS_CENTER,
                                     "resizable",       FALSE,
                                     NULL);
    gtk_container_add (GTK_CONTAINER (win), grid);
    gtk_widget_show_all (win);

    return g_application_run (gapp, argc, argv);
}
