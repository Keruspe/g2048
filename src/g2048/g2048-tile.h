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

#ifndef __G_2048_TILE_H__
#define __G_2048_TILE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define G_2048_VISIBLE __attribute__((visibility("default")))

#define G_2048_TYPE_TILE            (g_2048_tile_get_type ())
#define G_2048_TILE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_2048_TYPE_TILE, G2048Tile))
#define G_2048_IS_TILE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_2048_TYPE_TILE))
#define G_2048_TILE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), G_2048_TYPE_TILE, G2048TileClass))
#define G_2048_IS_TILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), G_2048_TYPE_TILE))
#define G_2048_TILE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), G_2048_TYPE_TILE, G2048TileClass))

typedef struct _G2048Tile G2048Tile;
typedef struct _G2048TileClass G2048TileClass;

G_2048_VISIBLE
GType g_2048_tile_get_type (void);

guint32 g_2048_tile_get_value (const G2048Tile *self);

void g_2048_tile_set_value (G2048Tile *self,
                            guint32    val);

GtkWidget *g_2048_tile_new (guint32 val);

G_END_DECLS

#endif /*__G_2048_TILE_H__*/
