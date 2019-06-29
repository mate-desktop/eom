/*
 * eom-application-activatable.c
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "eom-application-activatable.h"

#include <glib-object.h>
#include "eom-application.h"

G_DEFINE_INTERFACE(EomApplicationActivatable, eom_application_activatable, G_TYPE_OBJECT)

void
eom_application_activatable_default_init (EomApplicationActivatableInterface *iface)
{
	static gboolean initialized = FALSE;

	if (!initialized) {
		/**
         * EomApplicationActivatable:app:
		 *
         * This is the #EomApplication this #EomApplicationActivatable instance
		 * should be attached to.
		 */
		g_object_interface_install_property (iface,
                g_param_spec_object ("app", "Application",
                             "The EomApplication this instance it attached to",
                             EOM_TYPE_APPLICATION,
						     G_PARAM_READWRITE |
						     G_PARAM_CONSTRUCT_ONLY |
						     G_PARAM_STATIC_STRINGS));
		initialized = TRUE;
	}
}

void
eom_application_activatable_activate (EomApplicationActivatable *activatable)
{
    EomApplicationActivatableInterface *iface;

    g_return_if_fail (EOM_IS_APPLICATION_ACTIVATABLE (activatable));

    iface = EOM_APPLICATION_ACTIVATABLE_GET_IFACE (activatable);

	if (G_LIKELY (iface->activate != NULL))
		iface->activate (activatable);
}

void
eom_application_activatable_deactivate (EomApplicationActivatable *activatable)
{
    EomApplicationActivatableInterface *iface;

    g_return_if_fail (EOM_IS_APPLICATION_ACTIVATABLE (activatable));

    iface = EOM_APPLICATION_ACTIVATABLE_GET_IFACE (activatable);

	if (G_LIKELY (iface->deactivate != NULL))
		iface->deactivate (activatable);
}

