/*
 * eom-application-activatable.h
 * This file is part of eom
 *
 * Author: Felix Riemann <friemann@gnome.org>
 *
 * Copyright (C) 2012 Felix Riemann
 *
 * Base on code by:
 * 	- Steve Frécinaux <code@istique.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __EOM_APPLICATION_ACTIVATABLE_H__
#define __EOM_APPLICATION_ACTIVATABLE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define EOM_TYPE_APPLICATION_ACTIVATABLE (eom_application_activatable_get_type ())
#define EOM_APPLICATION_ACTIVATABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                          EOM_TYPE_APPLICATION_ACTIVATABLE, \
                                          EomApplicationActivatable))
#define EOM_APPLICATION_ACTIVATABLE_IFACE(obj) \
                                          (G_TYPE_CHECK_CLASS_CAST ((obj), \
                                           EOM_TYPE_APPLICATION_ACTIVATABLE, \
                                           EomApplicationActivatableInterface))
#define EOM_IS_APPLICATION_ACTIVATABLE(obj) \
                                          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           EOM_TYPE_APPLICATION_ACTIVATABLE))
#define EOM_APPLICATION_ACTIVATABLE_GET_IFACE(obj) \
                                        (G_TYPE_INSTANCE_GET_INTERFACE ((obj), \
                                         EOM_TYPE_APPLICATION_ACTIVATABLE, \
                                         EomApplicationActivatableInterface))

typedef struct _EomApplicationActivatable		EomApplicationActivatable;
typedef struct _EomApplicationActivatableInterface	EomApplicationActivatableInterface;

struct _EomApplicationActivatableInterface
{
	GTypeInterface g_iface;

	/* vfuncs */

    void	(*activate)	(EomApplicationActivatable *activatable);
    void	(*deactivate)	(EomApplicationActivatable *activatable);
};

GType	eom_application_activatable_get_type     (void) G_GNUC_CONST;

void	eom_application_activatable_activate     (EomApplicationActivatable *activatable);
void	eom_application_activatable_deactivate   (EomApplicationActivatable *activatable);

G_END_DECLS
#endif /* __EOM_APPLICATION_ACTIVATABLE_H__ */

