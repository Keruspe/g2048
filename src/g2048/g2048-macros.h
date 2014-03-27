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

#ifndef __G_2048_MACROS_H__
#define __G_2048_MACROS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define G_2048_VISIBLE __attribute__((visibility("default")))

#define G_2048_CLEANUP(fun) __attribute__((cleanup(fun)))

#define G_2048_CLEANUP_FREE         G_2048_CLEANUP (g_2048_free_ptr)
#define G_2048_CLEANUP_ERROR_FREE   G_2048_CLEANUP (g_2048_error_free_ptr)
#define G_2048_CLEANUP_OPTIONS_FREE G_2048_CLEANUP (g_2048_options_free_ptr)

#define G_2048_CLEANUP_UNREF        G_2048_CLEANUP (g_2048_unref_ptr)

#define G_2048_TRIVIAL_CLEANUP_FUN_FULL(name, type, fun, param_type) \
    static inline void                                               \
    g_2048_##name##_ptr (param_type ptr)                             \
    {                                                                \
        g_clear_pointer ((type *) ptr, fun);                         \
    }

#define G_2048_TRIVIAL_CLEANUP_FUN(name, type, fun) \
    G_2048_TRIVIAL_CLEANUP_FUN_FULL (name, type, fun, type *)

G_2048_TRIVIAL_CLEANUP_FUN_FULL (free,         gpointer,          g_free,         gpointer)

G_2048_TRIVIAL_CLEANUP_FUN      (error_free,   GError *,          g_error_free)
G_2048_TRIVIAL_CLEANUP_FUN      (options_free, GOptionContext *,  g_option_context_free)

G_2048_TRIVIAL_CLEANUP_FUN_FULL (unref,        GObject *,         g_object_unref, gpointer)

G_END_DECLS

#endif /*__G_2048_MACROS_H__*/
