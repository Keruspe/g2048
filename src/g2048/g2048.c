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

#include "g2048-tile.h"

#include <stdlib.h>

#define SIZE 4

static guint empty;

static void
_gtk_grid_add_row (GtkGrid *grid,
                   guint    row_nb,
                   gsize    size)
{
    GtkWidget *tile, *previous_tile;
    gtk_grid_attach (grid, (tile = g_2048_tile_new (0)), 0, row_nb, 1, 1);
    for (gsize s = 1; s < size; ++s)
    {
        previous_tile = tile;
        gtk_grid_attach_next_to (grid, (tile = g_2048_tile_new (0)), previous_tile, GTK_POS_RIGHT, 1, 1);
    }
}


static void
_gtk_grid_init (GtkGrid *grid,
                gsize    size)
{
    for (gsize s = 0; s < size; ++s)
        _gtk_grid_add_row (grid, s, size);
    empty = size * size;
}

static void
add_random_tile (GtkGrid *grid)
{
    gint32 r = g_random_int_range (0, empty);
    guint32 val = ((g_random_int_range (0, 99) > 90) ? 4 : 2);
    for (gsize s = 0; s < SIZE; ++s)
    {
        for (gsize ss = 0; ss < SIZE; ++ss)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, ss, s));
            if (!g_2048_tile_get_value (tile) && !(r--))
            {
                g_2048_tile_set_value (tile, val);
                --empty;
                return;
            }
        }
    }
}

static gboolean
on_up (GtkGrid *grid)
{
    g_debug ("up");
    return FALSE;
}

static gboolean
on_down (GtkGrid *grid)
{
    g_debug ("down");
    return FALSE;
}

static gboolean
on_left (GtkGrid *grid)
{
    g_debug ("left");
    return FALSE;
}

static gboolean
on_right (GtkGrid *grid)
{
    g_debug ("right");
    return FALSE;
}

static gboolean
on_key (GtkWidget   *widget,
        GdkEventKey *event)
{
    GtkGrid *grid = GTK_GRID (gtk_bin_get_child (GTK_BIN (widget)));
    gboolean won = FALSE;

    switch (event->keyval)
    {
    case GDK_KEY_Up:
        won = on_up (grid);
        break;
    case GDK_KEY_Down:
        won = on_down (grid);
        break;
    case GDK_KEY_Left:
        won = on_left (grid);
        break;
    case GDK_KEY_Right:
        won = on_right (grid);
        break;
    default:
        return FALSE;
    };

    if (won)
    {
        printf ("You win");
        exit (EXIT_SUCCESS);
    }

    if (!empty)
    {
        printf ("You lose");
        exit (EXIT_SUCCESS);
    }

    add_random_tile (grid);

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

    GtkWidget *grid = gtk_widget_new (GTK_TYPE_GRID,
                                      "column-homogeneous", TRUE,
                                      "row-homogeneous",    TRUE,
                                      "column-spacing",     10,
                                      "row-spacing",        10,
                                      "border-width",       15,
                                      NULL);
    GtkGrid *g = GTK_GRID (grid);
    _gtk_grid_init (g, SIZE);

    GtkWidget *win = gtk_widget_new (GTK_TYPE_APPLICATION_WINDOW,
                                     "application",     app,
                                     "type",            GTK_WINDOW_TOPLEVEL,
                                     "window-position", GTK_WIN_POS_CENTER,
                                     "resizable",       FALSE,
                                     NULL);
    gtk_container_add (GTK_CONTAINER (win), grid);
    gtk_widget_show_all (win);
    GTK_WIDGET_GET_CLASS (win)->key_press_event = on_key;

    add_random_tile (g);
    add_random_tile (g);

    return g_application_run (gapp, argc, argv);
}
