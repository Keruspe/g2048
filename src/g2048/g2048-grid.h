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

#ifndef __G_2048_GRID_H__
#define __G_2048_GRID_H__

#include "g2048-tile.h"

G_BEGIN_DECLS

#define G_2048_VISIBLE __attribute__((visibility("default")))

#define G_2048_TYPE_GRID            (g_2048_grid_get_type ())
#define G_2048_GRID(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_2048_TYPE_GRID, G2048Grid))
#define G_2048_IS_GRID(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_2048_TYPE_GRID))
#define G_2048_GRID_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), G_2048_TYPE_GRID, G2048GridClass))
#define G_2048_IS_GRID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), G_2048_TYPE_GRID))
#define G_2048_GRID_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), G_2048_TYPE_GRID, G2048GridClass))

typedef struct _G2048Grid G2048Grid;
typedef struct _G2048GridClass G2048GridClass;

G_2048_VISIBLE
GType g_2048_grid_get_type (void);

gboolean g_2048_grid_on_key (G2048Grid *self,
                             guint32    key);

gboolean g_2048_grid_is_won  (const G2048Grid *self);
gboolean g_2048_grid_is_lost (const G2048Grid *self);

GtkWidget *g_2048_grid_new (gsize     size,
                            GtkLabel *score_label);

G_END_DECLS

#endif /*__G_2048_GRID_H__*/
