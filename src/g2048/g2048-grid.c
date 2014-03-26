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

static inline gboolean
_cmp (gsize    a,
      gsize    b,
      gboolean reverse)
{
    if (a > (gsize) - 3 || b > (gsize) -3)
        return FALSE;
    return (reverse) ? (a < b) : (a > b);
}

static inline gboolean
_cmp_eq (gsize    a,
         gsize    b,
         gboolean reverse)
{
    return (a == b) || _cmp (a, b, reverse);
}

static gboolean
g_2048_grid_handle_vertical (G2048Grid *self,
                             gboolean   reverse)
{
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (self);
    GtkGrid *grid = GTK_GRID (self);
    gsize begin = (reverse) ? 0 : priv->size - 1;
    gsize end = (reverse) ? priv->size - 1 : 0;
    gsize delta = (reverse) ? 1 : -1;
    gboolean ret = FALSE;

    for (gsize col = begin; _cmp_eq (col, end, reverse); col += delta)
    {
        /* Stack everything */
        for (gsize row = begin; _cmp_eq (row, end, reverse); row += delta)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
            guint32 val = g_2048_tile_get_value (tile);
            if (val)
                continue;
            for (gsize rr = row + delta; _cmp_eq (rr, end, reverse); rr += delta)
            {
                G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, col, rr));
                val = g_2048_tile_get_value (next);
                if (val)
                {
                    g_2048_tile_set_value (tile, val);
                    g_2048_tile_set_value (next, 0);
                    break;
                }
            }
        }
        /* Merge what is mergeable */
        for (gsize row = begin; _cmp (row, end, reverse); row += delta)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
            guint32 val = g_2048_tile_get_value (tile);
            if (!val)
                break;
            G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, col, row + delta));
            if (val != g_2048_tile_get_value (next))
                continue;
            if (val == 1024)
                ret = TRUE;
            g_2048_tile_set_value (tile, val*2);
            g_2048_tile_set_value (next, 0);
            ++priv->empty;
            for (gsize rr = row + (2 * delta); _cmp (rr, end, reverse); rr += delta)
            {
                tile = next;
                next = G_2048_TILE (gtk_grid_get_child_at (grid, col, rr));
                val = g_2048_tile_get_value (next);
                if (!val)
                    break;
                g_2048_tile_set_value (tile, val);
                g_2048_tile_set_value (next, 0);
            }
        }
    }

    return ret;
}

static gboolean
g_2048_grid_handle_horizontal (G2048Grid *self,
                               gboolean reverse)
{
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (self);
    GtkGrid *grid = GTK_GRID (self);
    gsize begin = (reverse) ? 0 : priv->size - 1;
    gsize end = (reverse) ? priv->size - 1 : 0;
    gsize delta = (reverse) ? 1 : -1;
    gboolean ret = FALSE;

    for (gsize row = begin; _cmp_eq (row, end, reverse); row += delta)
    {
        /* Stack everything */
        for (gsize col = begin; _cmp_eq (col, end, reverse); col += delta)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
            guint32 val = g_2048_tile_get_value (tile);
            if (val)
                continue;
            for (gsize cc = col +delta; _cmp_eq (cc, end, reverse); cc += delta)
            {
                G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, cc, row));
                val = g_2048_tile_get_value (next);
                if (val)
                {
                    g_2048_tile_set_value (tile, val);
                    g_2048_tile_set_value (next, 0);
                    break;
                }
            }
        }
        /* Merge what is mergeable */
        for (gsize col = begin; _cmp (col, end, reverse); col += delta)
        {
            G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
            guint32 val = g_2048_tile_get_value (tile);
            if (!val)
                break;
            G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, col + delta, row));
            if (val != g_2048_tile_get_value (next))
            {
                continue;
            }
            if (val == 1024)
                ret = TRUE;
            g_2048_tile_set_value (tile, val*2);
            g_2048_tile_set_value (next, 0);
            ++priv->empty;
            for (gsize cc = col + (2 * delta); _cmp (cc, end, reverse); cc += delta)
            {
                tile = next;
                next = G_2048_TILE (gtk_grid_get_child_at (grid, cc, row));
                val = g_2048_tile_get_value (next);
                if (!val)
                    break;
                g_2048_tile_set_value (tile, val);
                g_2048_tile_set_value (next, 0);
            }
        }
    }

    return ret;
}

G_2048_VISIBLE gboolean
g_2048_grid_on_key (G2048Grid *self,
                    guint32    key)
{
    g_return_val_if_fail (G_2048_IS_GRID (self), FALSE);

    gboolean reverse = FALSE;
    gboolean ret = FALSE;

    switch (key)
    {
    case GDK_KEY_Up:
        reverse = TRUE;
    case GDK_KEY_Down:
        ret = g_2048_grid_handle_vertical (self, reverse);
        break;
    case GDK_KEY_Left:
        reverse = TRUE;
    case GDK_KEY_Right:
        ret = g_2048_grid_handle_horizontal (self, reverse);
        break;
    default:
        g_assert_not_reached ();
    }

    g_2048_grid_add_random_tile (self);
    return ret;
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
