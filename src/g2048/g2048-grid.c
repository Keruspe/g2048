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

#include <g2048-grid.h>

struct _G2048Grid
{
    GtkGrid parent_instance;
};

struct _G2048GridClass
{
    GtkGridClass parent_class;
};

typedef struct
{
    gsize   size;
    guint32 empty;
} G2048GridPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (G2048Grid, g_2048_grid, GTK_TYPE_GRID)

static void
g_2048_grid_add_random_tile (G2048Grid *self)
{
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (self);
    GtkGrid *grid = GTK_GRID (self);
    gint32 r = g_random_int_range (0, priv->empty);
    guint32 val = ((g_random_int_range (0, 99) > 90) ? 4 : 2);

    for (gsize row = 0; row < priv->size; ++row)
    {
        for (gsize col = 0; col < priv->size; ++col)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
            if (!g_2048_tile_get_value (tile) && !(r--))
            {
                g_2048_tile_set_value (tile, val);
                --priv->empty;
                return;
            }
        }
    }
    g_assert_not_reached ();
}

G_2048_VISIBLE gboolean
g_2048_grid_on_key (G2048Grid *self,
                    guint32    key)
{
    g_return_val_if_fail (G_2048_IS_GRID (self), FALSE);
    g_debug ("key");
    g_2048_grid_add_random_tile (self);
    return FALSE;
}

G_2048_VISIBLE gboolean
g_2048_grid_is_full (const G2048Grid *self)
{
    g_return_val_if_fail (G_2048_IS_GRID (self), TRUE);
    G2048GridPrivate *priv = g_2048_grid_get_instance_private ((G2048Grid *) self);
    return !priv->empty;
}

static void
g_2048_grid_class_init (G2048GridClass *klass G_GNUC_UNUSED)
{
}

static void
g_2048_grid_init (G2048Grid *self)
{
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (self);

    priv->empty = 0;
}

G_2048_VISIBLE GtkWidget *
g_2048_grid_new (gsize size)
{
    GtkWidget *self = gtk_widget_new (G_2048_TYPE_GRID,
                                      "column-homogeneous", TRUE,
                                      "row-homogeneous",    TRUE,
                                      "column-spacing",     10,
                                      "row-spacing",        10,
                                      "border-width",       20,
                                      NULL);
    G2048Grid *grid = (G2048Grid *) self;
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (grid);
    GtkGrid *g = GTK_GRID (self);

    priv->size = size;
    priv->empty = size*size;

    for (gsize row = 0; row < size; ++row)
    {
        GtkWidget *tile, *previous_tile;
        gtk_grid_attach (g, (tile = g_2048_tile_new (0)), 0, row, 1, 1);
        for (gsize col = 1; col < size; ++col)
        {
            previous_tile = tile;
            gtk_grid_attach_next_to (g, (tile = g_2048_tile_new (0)), previous_tile, GTK_POS_RIGHT, 1, 1);
        }
    }

    g_2048_grid_add_random_tile (grid);
    g_2048_grid_add_random_tile (grid);

    return self;
}
