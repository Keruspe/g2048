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

#define DEFAULT_THEME "default"

struct _G2048Tile
{
    GtkImage parent_instance;
};

struct _G2048TileClass
{
    GtkImageClass parent_class;
};

typedef struct
{
    guint32 value;
} G2048TilePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (G2048Tile, g_2048_tile, GTK_TYPE_LABEL)

static void
g_2048_tile_update (G2048Tile *self,
                    guint32    value)
{
    gchar *file = g_strdup_printf (DATADIR "/" PACKAGE_NAME "/%s/%u.png", DEFAULT_THEME, value);
    gtk_image_set_from_file (GTK_IMAGE (self), file);
    g_free (file);
}

G_2048_VISIBLE guint32
g_2048_tile_get_value (const G2048Tile *self)
{
    g_return_val_if_fail (G_2048_IS_TILE (self), 0);
    G2048TilePrivate *priv = g_2048_tile_get_instance_private ((G2048Tile *) self);
    return priv->value;
}

G_2048_VISIBLE void
g_2048_tile_set_value (G2048Tile *self,
                       guint32    val)
{
    g_return_if_fail (G_2048_IS_TILE (self));
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (self);
    priv->value = val;
    g_2048_tile_update (self, val);
}

static void
g_2048_tile_class_init (G2048TileClass *klass G_GNUC_UNUSED)
{
}

static void
g_2048_tile_init (G2048Tile *self)
{
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (self);
    priv->value = 0;
}

G_2048_VISIBLE GtkWidget *
g_2048_tile_new (guint32 val)
{
    GtkWidget *self = gtk_widget_new (G_2048_TYPE_TILE,
                                      "height-request", 100,
                                      "width-request",  100,
                                      NULL);
    G2048Tile *tile = (G2048Tile *) self;
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (tile);
    priv->value = val;
    g_2048_tile_update (tile, val);
    return self;
}
