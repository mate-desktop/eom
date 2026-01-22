# EOM Plugin Development Guide

Quick guide for creating plugins for Eye of MATE (EOM).

## Quick Start

EOM uses **libpeas** for plugins. A plugin extends EOM by implementing the `EomWindowActivatable` interface.

**What you can do:**
- Add menu items and toolbar buttons
- React to image changes
- Access image data and metadata
- Add custom UI widgets

**Languages:** C or Python 3

**Install location:** `~/.local/share/eom/plugins/your-plugin-name/`

## Plugin Structure

Every plugin needs these files:

1. **Plugin header** - `my-plugin.h`
2. **Plugin code** - `my-plugin.c`
3. **Plugin descriptor** - `my-plugin.plugin`
4. **Build file** - `Makefile`

### Basic Plugin Template

**Note:** This is a simplified template. For a complete working example with all necessary boilerplate, see `plugins/reload/` in the EOM source tree.

```c
// my-plugin.h
#ifndef __MY_PLUGIN_H__
#define __MY_PLUGIN_H__

#include <glib-object.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>
#include <eom-window.h>

G_BEGIN_DECLS

#define MY_TYPE_PLUGIN (my_plugin_get_type ())
G_DECLARE_FINAL_TYPE (MyPlugin, my_plugin, MY, PLUGIN, PeasExtensionBase)

struct _MyPlugin {
    PeasExtensionBase parent;
    EomWindow *window;
    GtkActionGroup *action_group;
    guint ui_id;
};

GType my_plugin_get_type (void) G_GNUC_CONST;
G_MODULE_EXPORT void peas_register_types (PeasObjectModule *module);

G_END_DECLS

#endif /* __MY_PLUGIN_H__ */
```

```c
// my-plugin.c
#include "my-plugin.h"
#include <eom-window-activatable.h>

static void eom_window_activatable_iface_init (EomWindowActivatableInterface *iface);

// Register the plugin type
G_DEFINE_DYNAMIC_TYPE_EXTENDED (MyPlugin, my_plugin,
    PEAS_TYPE_EXTENSION_BASE, 0,
    G_IMPLEMENT_INTERFACE_DYNAMIC (EOM_TYPE_WINDOW_ACTIVATABLE,
                                   eom_window_activatable_iface_init))

enum {
    PROP_0,
    PROP_WINDOW
};

// Initialize plugin instance
static void
my_plugin_init (MyPlugin *plugin)
{
    // Initialize your plugin data here
}

// Clean up plugin resources
static void
my_plugin_dispose (GObject *object)
{
    MyPlugin *plugin = MY_PLUGIN (object);

    if (plugin->window != NULL) {
        g_object_unref (plugin->window);
        plugin->window = NULL;
    }

    G_OBJECT_CLASS (my_plugin_parent_class)->dispose (object);
}

// Set plugin properties
static void
my_plugin_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    MyPlugin *plugin = MY_PLUGIN (object);

    switch (prop_id) {
    case PROP_WINDOW:
        plugin->window = EOM_WINDOW (g_value_dup_object (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

// Get plugin properties
static void
my_plugin_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    MyPlugin *plugin = MY_PLUGIN (object);

    switch (prop_id) {
    case PROP_WINDOW:
        g_value_set_object (value, plugin->window);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

// Called when plugin is enabled
static void
my_plugin_activate (EomWindowActivatable *activatable)
{
    MyPlugin *plugin = MY_PLUGIN (activatable);

    // Add menu items, connect signals, etc.
}

// Called when plugin is disabled
static void
my_plugin_deactivate (EomWindowActivatable *activatable)
{
    MyPlugin *plugin = MY_PLUGIN (activatable);

    // Remove menu items, disconnect signals, etc.
}

// Initialize plugin class
static void
my_plugin_class_init (MyPluginClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = my_plugin_dispose;
    object_class->set_property = my_plugin_set_property;
    object_class->get_property = my_plugin_get_property;

    g_object_class_override_property (object_class, PROP_WINDOW, "window");
}

// Required by G_DEFINE_DYNAMIC_TYPE_EXTENDED
static void
my_plugin_class_finalize (MyPluginClass *klass)
{
    // Dummy function for dynamic type registration
}

// Implement the activatable interface
static void
eom_window_activatable_iface_init (EomWindowActivatableInterface *iface)
{
    iface->activate = my_plugin_activate;
    iface->deactivate = my_plugin_deactivate;
}

// Register with libpeas
G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
    my_plugin_register_type (G_TYPE_MODULE (module));
    peas_object_module_register_extension_type (module,
        EOM_TYPE_WINDOW_ACTIVATABLE, MY_TYPE_PLUGIN);
}
```

### Plugin Descriptor

```ini
# my-plugin.plugin
[Plugin]
Module=my-plugin
IAge=2
Name=My Plugin Name
Description=What this plugin does
Authors=Your Name <email@example.com>
Copyright=Copyright © 2025 Your Name
Website=https://mate-desktop.org
```

### Build File

```makefile
# Makefile
PLUGIN_NAME = my-plugin
SOURCES = my-plugin.c
HEADERS = my-plugin.h

USER_DIR = $(HOME)/.local/share/eom/plugins/$(PLUGIN_NAME)
CFLAGS = `pkg-config --cflags gtk+-3.0 eom libpeas-1.0` -fPIC
LDFLAGS = `pkg-config --libs gtk+-3.0 eom libpeas-1.0` -shared

all: lib$(PLUGIN_NAME).so

lib$(PLUGIN_NAME).so: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS)

install: all
	mkdir -p $(USER_DIR)
	cp lib$(PLUGIN_NAME).so $(USER_DIR)/
	cp $(PLUGIN_NAME).plugin $(USER_DIR)/

clean:
	rm -f lib$(PLUGIN_NAME).so
```

## Common Tasks

### Adding a Menu Item

```c
static void
my_action_callback (GtkAction *action, EomWindow *window)
{
    // Do something when menu item is clicked
}

static const GtkActionEntry actions[] = {
    { "MyAction", NULL, "Menu Label", "<Ctrl>M",
      "Tooltip text", G_CALLBACK (my_action_callback) }
};

static const gchar* ui_xml =
    "<ui><menubar name='MainMenu'>"
    "<menu name='ToolsMenu' action='Tools'>"
    "<menuitem action='MyAction'/>"
    "</menu></menubar></ui>";

static void
my_plugin_activate (EomWindowActivatable *activatable)
{
    MyPlugin *plugin = MY_PLUGIN (activatable);
    GtkUIManager *manager = eom_window_get_ui_manager (plugin->window);

    plugin->action_group = gtk_action_group_new ("MyActions");
    gtk_action_group_add_actions (plugin->action_group, actions,
                                  G_N_ELEMENTS (actions), plugin->window);
    gtk_ui_manager_insert_action_group (manager, plugin->action_group, -1);
    plugin->ui_id = gtk_ui_manager_add_ui_from_string (manager, ui_xml, -1, NULL);
}
```

### Working with Images

```c
// Get current image
EomImage *image = eom_window_get_image (window);

// Get image data
GdkPixbuf *pixbuf = eom_image_get_pixbuf (image);
gint width, height;
eom_image_get_size (image, &width, &height);

// Check image type
if (eom_image_is_jpeg (image)) { /* ... */ }

// Transform image
EomTransform *transform = eom_transform_rotate_new (90);
eom_image_transform (image, transform, NULL);
g_object_unref (transform);

// Mark as modified
eom_image_modified (image);
```

### Responding to Events

```c
static void
on_image_changed (EomThumbView *view, MyPlugin *plugin)
{
    EomImage *image = eom_window_get_image (plugin->window);
    // React to image change
}

static void
my_plugin_activate (EomWindowActivatable *activatable)
{
    MyPlugin *plugin = MY_PLUGIN (activatable);
    GtkWidget *thumbview = eom_window_get_thumb_view (plugin->window);

    plugin->signal_id = g_signal_connect (thumbview, "selection_changed",
                                          G_CALLBACK (on_image_changed), plugin);
}

static void
my_plugin_deactivate (EomWindowActivatable *activatable)
{
    MyPlugin *plugin = MY_PLUGIN (activatable);
    GtkWidget *thumbview = eom_window_get_thumb_view (plugin->window);

    g_signal_handler_disconnect (thumbview, plugin->signal_id);
}
```

## Key APIs

### EomWindow

```c
EomImage *     eom_window_get_image      (EomWindow *window);
GtkUIManager * eom_window_get_ui_manager (EomWindow *window);
GtkWidget *    eom_window_get_view       (EomWindow *window);
GtkWidget *    eom_window_get_sidebar    (EomWindow *window);
GtkWidget *    eom_window_get_statusbar  (EomWindow *window);
void           eom_window_reload_image   (EomWindow *window);
```

### EomImage

```c
GdkPixbuf * eom_image_get_pixbuf  (EomImage *img);
void        eom_image_get_size    (EomImage *img, gint *width, gint *height);
void        eom_image_transform   (EomImage *img, EomTransform *trans, EomJob *job);
void        eom_image_modified    (EomImage *img);
gboolean    eom_image_is_jpeg     (EomImage *img);
```

See `src/eom-window.h` and `src/eom-image.h` for complete API.

## Building and Testing

```bash
# Build the plugin
make

# Install to user directory
make install

# Test
eom
# Go to Edit → Preferences → Plugins
# Enable your plugin
```

## Built-in Examples

Study these plugins in the `plugins/` directory:

1. **fullscreen** - Simple event handling (double-click to toggle fullscreen)
2. **reload** - Adding menu items (reload image from disk)
3. **statusbar-date** - Widget manipulation and EXIF reading

## Tips

- **Memory management**: Always `g_object_unref()` objects and `g_free()` allocated memory
- **Cleanup**: Remove all UI elements and disconnect all signals in `deactivate()`
- **Debugging**: Run with `EOM_DEBUG_PLUGINS=1 eom` or `EOM_DEBUG=1 eom` to see plugin messages
- **Headers**: All EOM headers are in `/usr/include/eom/` (install `libeom-dev`)

## Complete Example

See `plugins/reload/` in the EOM source for a complete, simple example showing:
- Plugin structure with proper GObject boilerplate
- Adding a menu item with keyboard shortcut
- Calling EOM window functions
- Proper activation and deactivation

## Resources

- **libpeas source/examples**: https://gitlab.gnome.org/GNOME/libpeas/-/tree/1.36?ref_type=heads
- **EOM headers**: `/usr/include/eom/` or `src/` in source tree (install `libeom-dev`)
- **GTK+ 3 docs**: https://docs.gtk.org/gtk3/
- **MATE wiki**: https://wiki.mate-desktop.org/
- **GObject Tutorial**: https://docs.gtk.org/gobject/

## Next Steps

1. Copy one of the built-in plugins as a template
2. Modify it for your needs
3. Build and test iteratively
4. Share your plugin with the community!

For advanced topics (GSettings integration, complex UI, Python plugins), refer to the built-in plugin code and GTK+/libpeas documentation.
