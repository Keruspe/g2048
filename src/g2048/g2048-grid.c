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
    gsize     size;
    guint32   empty;

    guint32   score;

    GtkLabel *score_label;
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
g_2048_grid_replace_by_non_empty (GtkGrid   *grid,
                                  G2048Tile *tile,
                                  gsize      col,
                                  gsize      row,
                                  gsize      end,
                                  gsize      delta,
                                  gboolean   reverse,
                                  gboolean   vertical)
{
    for (gsize ll = ((vertical) ? row : col) + delta; _cmp_eq (ll, end, reverse); ll += delta)
    {
        col = (vertical) ? col : ll;
        row = (vertical) ? ll : row;
        G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
        guint32 val = g_2048_tile_get_value (next);
        if (val)
        {
            g_2048_tile_set_value (tile, val);
            g_2048_tile_set_value (next, 0);
            return TRUE;
        }
    }
    return FALSE;
}

static void
g_2048_grid_stack (GtkGrid *grid,
                   gsize    nb,
                   gsize    begin,
                   gsize    end,
                   gsize    delta,
                   gboolean reverse,
                   gboolean vertical)
{
    for (gsize line = begin; _cmp_eq (line, end, reverse); line += delta)
    {
        gsize col = (vertical) ? nb : line;
        gsize row = (vertical) ? line : nb;
        G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
        if (!g_2048_tile_get_value (tile) &&
            !g_2048_grid_replace_by_non_empty (grid, tile, col, row, end, delta, reverse, vertical))
                break;
    }
}

static void
g_2048_grid_update_score (GtkLabel *score_label,
                          guint32  *score,
                          guint32   points)
{
    *score += points;
    gchar *txt = g_strdup_printf ("%u", *score);
    gtk_label_set_text (score_label, txt);
    g_free (txt);
}

static gboolean
g_2048_grid_post_merge (GtkGrid   *grid,
                        GtkLabel  *score_label,
                        guint32   *score,
                        G2048Tile *next,
                        gsize      row,
                        gsize      col,
                        gsize      end,
                        gsize      delta,
                        gboolean   reverse,
                        gboolean   vertical)
{
    gboolean found = FALSE;

    for (gsize ll = ((vertical) ? row : col) + (2 * delta); _cmp_eq (ll, end, reverse); ll += delta)
    {
        col = (vertical) ? col : ll;
        row = (vertical) ? ll : row;
        G2048Tile *tile = next;
        next = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
        guint32 val = g_2048_tile_get_value (next);
        if (val)
        {
            found = TRUE;
            if (_cmp (ll + delta, end, reverse))
            {
                col = (vertical) ? col : (ll + delta);
                row = (vertical) ? (ll + delta) : row;
                G2048Tile *_next = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
                if (g_2048_tile_get_value (_next) == val)
                {
                    val *= 2;
                    g_2048_grid_update_score (score_label, score, val);
                    g_2048_tile_set_value (_next, 0);
                }
            }
        }
        g_2048_tile_set_value (tile, val);
        g_2048_tile_set_value (next, 0);
    }

    return found;
}

static gboolean
g_2048_grid_merge (GtkGrid  *grid,
                   GtkLabel *score_label,
                   guint32  *score,
                   guint32  *empty,
                   gsize     nb,
                   gsize     begin,
                   gsize     end,
                   gsize     delta,
                   gboolean  reverse,
                   gboolean  vertical)
{
    gboolean ret = FALSE;

    for (gsize line = begin; _cmp (line, end, reverse); line += delta)
    {
        gsize col = (vertical) ? nb : line;
        gsize row = (vertical) ? line : nb;
        G2048Tile *tile = G_2048_TILE (gtk_grid_get_child_at (grid, col, row));
        guint32 val = g_2048_tile_get_value (tile);
        if (!val)
            break;
        gsize ncol = (vertical) ? col : (col + delta);
        gsize nrow = (vertical) ? (row + delta) : row;
        G2048Tile *next = G_2048_TILE (gtk_grid_get_child_at (grid, ncol, nrow));
        if (val != g_2048_tile_get_value (next))
            continue;
        if (val == 1024)
            ret = TRUE;
        val *= 2;
        g_2048_tile_set_value (tile, val);
        g_2048_tile_set_value (next, 0);
        g_2048_grid_update_score (score_label, score, val);
        ++(*empty);
        if (!g_2048_grid_post_merge (grid, score_label, score, next, row, col, end, delta, reverse, vertical))
            break;
    }

    return ret;
}

static gboolean g_2048_grid_handle (G2048Grid *self,
                                    gboolean   reverse,
                                    gboolean   vertical)
{
    G2048GridPrivate *priv = g_2048_grid_get_instance_private (self);
    GtkGrid *grid = GTK_GRID (self);
    gsize begin = (reverse) ? 0 : priv->size - 1;
    gsize end = (reverse) ? priv->size - 1 : 0;
    gsize delta = (reverse) ? 1 : -1;

    for (gsize line = begin; _cmp_eq (line, end, reverse); line += delta)
    {
        /* Stack everything */
        g_2048_grid_stack (grid, line, begin, end, delta, reverse, vertical);
        /* Merge what is mergeable */
        if (g_2048_grid_merge (grid, priv->score_label, &priv->score, &priv->empty, line, begin, end, delta, reverse, vertical))
            return TRUE;
    }

    return FALSE;
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
        ret = g_2048_grid_handle (self, reverse, TRUE);
        break;
    case GDK_KEY_Left:
        reverse = TRUE;
    case GDK_KEY_Right:
        ret = g_2048_grid_handle (self, reverse, FALSE);
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
g_2048_grid_new (gsize     size,
                 GtkLabel *score_label)
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

    priv->score_label = score_label;

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