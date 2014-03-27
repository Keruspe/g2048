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

    gchar  *theme;
} G2048TilePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (G2048Tile, g_2048_tile, GTK_TYPE_IMAGE)

static void
g_2048_tile_update (G2048Tile *self)
{
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (self);
    gchar *file = g_strdup_printf (DATADIR "/" PACKAGE_NAME "/themes/%s/%u", priv->theme, priv->value);
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
    g_2048_tile_update (self);
}

static void
g_2048_tile_finalize (GObject *object)
{
    G2048TilePrivate *priv = g_2048_tile_get_instance_private ((G2048Tile *) object);
    g_clear_pointer (&priv->theme, g_free);
    G_OBJECT_CLASS (g_2048_tile_parent_class)->finalize (object);
}

static void
g_2048_tile_class_init (G2048TileClass *klass)
{
    G_OBJECT_CLASS (klass)->finalize = g_2048_tile_finalize;
}

static void
g_2048_tile_init (G2048Tile *self)
{
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (self);
    priv->value = 0;
}

G_2048_VISIBLE GtkWidget *
g_2048_tile_new (guint32      val,
                 const gchar *theme)
{
    g_return_val_if_fail (theme, NULL);

    GtkWidget *self = gtk_widget_new (G_2048_TYPE_TILE,
                                      "height-request", 100,
                                      "width-request",  100,
                                      NULL);
    G2048Tile *tile = (G2048Tile *) self;
    G2048TilePrivate *priv = g_2048_tile_get_instance_private (tile);
    priv->value = val;
    priv->theme = g_strdup (theme);
    g_2048_tile_update (tile);
    return self;
}
