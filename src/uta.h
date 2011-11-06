/* Eye of Mate image viewer - Microtile array utilities
 *
 * Copyright (C) 2000-2009 The Free Software Foundation
 *
 * Author: Federico Mena-Quintero <federico@gnu.org>
 *
 * Portions based on code from libart_lgpl by Raph Levien.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef UTA_H
#define UTA_H

#define EOM_UTILE_SHIFT 5
#define EOM_UTILE_SIZE (1 << EOM_UTILE_SHIFT)

typedef guint32 EomUtaBbox;

struct _EomIRect {
  int x0, y0, x1, y1;
};

struct _EomUta {
  int x0;
  int y0;
  int width;
  int height;
  EomUtaBbox *utiles;
};

typedef struct _EomIRect EomIRect;
typedef struct _EomUta EomUta;



G_GNUC_INTERNAL
void	eom_uta_free 		(EomUta *uta);

G_GNUC_INTERNAL
void	eom_irect_intersect 	(EomIRect *dest,
				 const EomIRect *src1, const EomIRect *src2);
G_GNUC_INTERNAL
int	eom_irect_empty 	(const EomIRect *src);

G_GNUC_INTERNAL
EomUta *uta_ensure_size (EomUta *uta, int x1, int y1, int x2, int y2);

G_GNUC_INTERNAL
EomUta *uta_add_rect (EomUta *uta, int x1, int y1, int x2, int y2);

G_GNUC_INTERNAL
void uta_remove_rect (EomUta *uta, int x1, int y1, int x2, int y2);

G_GNUC_INTERNAL
void uta_find_first_glom_rect (EomUta *uta, EomIRect *rect, int max_width, int max_height);

G_GNUC_INTERNAL
void uta_copy_area (EomUta *uta, int src_x, int src_y, int dest_x, int dest_y, int width, int height);



#endif
