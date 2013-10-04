#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <string.h>
#include <glib.h>

#include "eom-uri-converter.h"
#include "eom-pixbuf-util.h"

enum {
	PROP_0,
	PROP_CONVERT_SPACES,
	PROP_SPACE_CHARACTER,
	PROP_COUNTER_START,
	PROP_COUNTER_N_DIGITS,
	PROP_N_IMAGES
};

typedef struct {
	EomUCType  type;
	union {
		char    *string;  /* if type == EOM_UC_STRING */
		gulong  counter;  /* if type == EOM_UC_COUNTER */
	} data;
} EomUCToken;


struct _EomURIConverterPrivate {
	GFile           *base_file;
	GList           *token_list;
	char            *suffix;
	GdkPixbufFormat *img_format;
	gboolean        requires_exif;

	/* options */
	gboolean convert_spaces;
	gchar    space_character;
	gulong   counter_start;
	guint    counter_n_digits;
};

static void eom_uri_converter_set_property (GObject      *object,
					    guint         property_id,
					    const GValue *value,
					    GParamSpec   *pspec);

static void eom_uri_converter_get_property (GObject    *object,
					    guint       property_id,
					    GValue     *value,
					    GParamSpec *pspec);

G_DEFINE_TYPE_WITH_PRIVATE (EomURIConverter, eom_uri_converter, G_TYPE_OBJECT)

static void
free_token (gpointer data)
{
	EomUCToken *token = (EomUCToken*) data;

	if (token->type == EOM_UC_STRING) {
		g_free (token->data.string);
	}

	g_slice_free (EomUCToken, token);
}

static void
eom_uri_converter_dispose (GObject *object)
{
	EomURIConverter *instance = EOM_URI_CONVERTER (object);
	EomURIConverterPrivate *priv;

	priv = instance->priv;

	if (priv->base_file) {
		g_object_unref (priv->base_file);
		priv->base_file = NULL;
	}

	if (priv->token_list) {
		g_list_foreach (priv->token_list, (GFunc) free_token, NULL);
		g_list_free (priv->token_list);
		priv->token_list = NULL;
	}

	if (priv->suffix) {
		g_free (priv->suffix);
		priv->suffix = NULL;
	}


	G_OBJECT_CLASS (eom_uri_converter_parent_class)->dispose (object);
}

static void
eom_uri_converter_init (EomURIConverter *conv)
{
	EomURIConverterPrivate *priv;

	priv = conv->priv = eom_uri_converter_get_instance_private (conv);

	priv->convert_spaces   = FALSE;
	priv->space_character  = '_';
	priv->counter_start    = 0;
	priv->counter_n_digits = 1;
	priv->requires_exif     = FALSE;
}

static void
eom_uri_converter_class_init (EomURIConverterClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;

	object_class->dispose = eom_uri_converter_dispose;

        /* GObjectClass */
        object_class->set_property = eom_uri_converter_set_property;
        object_class->get_property = eom_uri_converter_get_property;

        /* Properties */
        g_object_class_install_property (
                object_class,
                PROP_CONVERT_SPACES,
                g_param_spec_boolean ("convert-spaces", NULL, NULL,
				      FALSE, G_PARAM_READWRITE));

        g_object_class_install_property (
                object_class,
                PROP_SPACE_CHARACTER,
                g_param_spec_char ("space-character", NULL, NULL,
				   ' ', '~', '_', G_PARAM_READWRITE));

       g_object_class_install_property (
                object_class,
                PROP_COUNTER_START,
                g_param_spec_ulong ("counter-start", NULL, NULL,
                                   0,
                                   G_MAXULONG,
                                   1,
                                   G_PARAM_READWRITE));

       g_object_class_install_property (
                object_class,
                PROP_COUNTER_N_DIGITS,
                g_param_spec_uint ("counter-n-digits", NULL, NULL,
				  1,
				  G_MAXUINT,
				  1,
				  G_PARAM_READWRITE));


       g_object_class_install_property (
                object_class,
                PROP_N_IMAGES,
                g_param_spec_uint ("n-images", NULL, NULL,
				  1,
				  G_MAXUINT,
				  1,
				  G_PARAM_WRITABLE));
}

GQuark
eom_uc_error_quark (void)
{
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string ("eom-uri-converter-error-quark");

	return q;
}


static void
eom_uri_converter_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
	EomURIConverter *conv;
	EomURIConverterPrivate *priv;

        g_return_if_fail (EOM_IS_URI_CONVERTER (object));

        conv = EOM_URI_CONVERTER (object);
	priv = conv->priv;

        switch (property_id)
        {
	case PROP_CONVERT_SPACES:
		priv->convert_spaces = g_value_get_boolean (value);
		break;

	case PROP_SPACE_CHARACTER:
		priv->space_character = g_value_get_schar (value);
		break;

	case PROP_COUNTER_START:
	{
		guint new_n_digits;

		priv->counter_start = g_value_get_ulong (value);

		new_n_digits = ceil (log10 (priv->counter_start + pow (10, priv->counter_n_digits) - 1));

		if (new_n_digits != priv->counter_n_digits) {
			priv->counter_n_digits = ceil (MIN (log10 (G_MAXULONG), new_n_digits));
		}
		break;
	}

	case PROP_COUNTER_N_DIGITS:
		priv->counter_n_digits = ceil (MIN (log10 (G_MAXULONG), g_value_get_uint (value)));
		break;

	case PROP_N_IMAGES:
		priv->counter_n_digits = ceil (MIN (log10 (G_MAXULONG),
						    log10 (priv->counter_start + g_value_get_uint (value))));
		break;

        default:
                g_assert_not_reached ();
        }
}

static void
eom_uri_converter_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
	EomURIConverter *conv;
	EomURIConverterPrivate *priv;

        g_return_if_fail (EOM_IS_URI_CONVERTER (object));

        conv = EOM_URI_CONVERTER (object);
	priv = conv->priv;

        switch (property_id)
        {
	case PROP_CONVERT_SPACES:
		g_value_set_boolean (value, priv->convert_spaces);
		break;

	case PROP_SPACE_CHARACTER:
		g_value_set_schar (value, priv->space_character);
		break;

	case PROP_COUNTER_START:
		g_value_set_ulong (value, priv->counter_start);
		break;

	case PROP_COUNTER_N_DIGITS:
		g_value_set_uint (value, priv->counter_n_digits);
		break;

        default:
                g_assert_not_reached ();
	}
}

/* parser states */
enum {
	PARSER_NONE,
	PARSER_STRING,
	PARSER_TOKEN
};

static EomUCToken*
create_token_string (const char *string, int substr_start, int substr_len)
{
	char *start_byte;
	char *end_byte;
	int n_bytes;
	EomUCToken *token;

	if (string == NULL) return NULL;
	if (substr_len <= 0) return NULL;

	start_byte = g_utf8_offset_to_pointer (string, substr_start);
	end_byte = g_utf8_offset_to_pointer (string, substr_start + substr_len);

	/* FIXME: is this right? */
	n_bytes = end_byte - start_byte;

	token = g_slice_new0 (EomUCToken);
	token->type = EOM_UC_STRING;
	token->data.string = g_new0 (char, n_bytes);
	token->data.string = g_utf8_strncpy (token->data.string, start_byte, substr_len);

	return token;
}

static EomUCToken*
create_token_counter (int start_counter)
{
	EomUCToken *token;

	token = g_slice_new0 (EomUCToken);
	token->type = EOM_UC_COUNTER;
	token->data.counter = 0;

	return token;
}

static EomUCToken*
create_token_other (EomUCType type)
{
	EomUCToken *token;

	token = g_slice_new0 (EomUCToken);
	token->type = type;

	return token;
}

static GList*
eom_uri_converter_parse_string (EomURIConverter *conv, const char *string)
{
	EomURIConverterPrivate *priv;
	GList *list = NULL;
	gulong len;
	int i;
	int state = PARSER_NONE;
	int start = -1;
	int substr_len = 0;
	gunichar c;
	const char *s;
	EomUCToken *token;

	g_return_val_if_fail (EOM_IS_URI_CONVERTER (conv), NULL);

	priv = conv->priv;

	if (string == NULL) return NULL;

	if (!g_utf8_validate (string, -1, NULL))
		return NULL;

	len = g_utf8_strlen (string, -1);
	s = string;

	for (i = 0; i < len; i++) {
		c = g_utf8_get_char (s);
		token = NULL;

		switch (state) {
		case PARSER_NONE:
			if (c == '%') {
				start = -1;
				state = PARSER_TOKEN;
			} else {
				start = i;
				substr_len = 1;
				state = PARSER_STRING;
			}
			break;

		case PARSER_STRING:
			if (c == '%') {
				if (start != -1) {
					token = create_token_string (string, start, substr_len);
				}

				state = PARSER_TOKEN;
				start = -1;
			} else {
				substr_len++;
			}
			break;

		case PARSER_TOKEN: {
			EomUCType type = EOM_UC_END;

			if (c == 'f') {
				type = EOM_UC_FILENAME;
			}
			else if (c == 'n') {
				type = EOM_UC_COUNTER;
				token = create_token_counter (priv->counter_start);
			}
			else if (c == 'c') {
				type = EOM_UC_COMMENT;
			}
			else if (c == 'd') {
				type = EOM_UC_DATE;
			}
			else if (c == 't') {
				type = EOM_UC_TIME;
			}
			else if (c == 'a') {
				type = EOM_UC_DAY;
			}
			else if (c == 'm') {
				type = EOM_UC_MONTH;
			}
			else if (c == 'y') {
				type = EOM_UC_YEAR;
			}
			else if (c == 'h') {
				type = EOM_UC_HOUR;
			}
			else if (c == 'i') {
				type = EOM_UC_MINUTE;
			}
			else if (c == 's') {
				type = EOM_UC_SECOND;
			}

			if (type != EOM_UC_END && token == NULL) {
				token = create_token_other (type);
				priv->requires_exif = TRUE;
			}
			state = PARSER_NONE;
			break;
		}
		default:
			g_assert_not_reached ();
		}


		if (token != NULL) {
			list = g_list_append (list, token);
		}

		s = g_utf8_next_char (s);
	}

	if (state != PARSER_TOKEN && start >= 0) {
		/* add remaining chars as string token */
		list = g_list_append (list, create_token_string (string, start, substr_len));
	}

	return list;
}

void
eom_uri_converter_print_list (EomURIConverter *conv)
{
	EomURIConverterPrivate *priv;
	GList *it;

	g_return_if_fail (EOM_URI_CONVERTER (conv));

	priv = conv->priv;

	for (it = priv->token_list; it != NULL; it = it->next) {
		EomUCToken *token;
		char *str;

		token = (EomUCToken*) it->data;

		switch (token->type) {
		case EOM_UC_STRING:
			str = g_strdup_printf ("string [%s]", token->data.string);
			break;
		case EOM_UC_FILENAME:
			str = "filename";
			break;
		case EOM_UC_COUNTER:
			str = g_strdup_printf ("counter [%lu]", token->data.counter);
			break;
		case EOM_UC_COMMENT:
			str = "comment";
			break;
		case EOM_UC_DATE:
			str = "date";
			break;
		case EOM_UC_TIME:
			str = "time";
			break;
		case EOM_UC_DAY:
			str = "day";
			break;
		case EOM_UC_MONTH:
			str = "month";
			break;
		case EOM_UC_YEAR:
			str = "year";
			break;
		case EOM_UC_HOUR:
			str = "hour";
			break;
		case EOM_UC_MINUTE:
			str = "minute";
			break;
		case EOM_UC_SECOND:
			str = "second";
			break;
		default:
			str = "unknown";
			break;
		}

		g_print ("- %s\n", str);

		if (token->type == EOM_UC_STRING || token->type == EOM_UC_COUNTER) {
			g_free (str);
		}
	}
}


EomURIConverter*
eom_uri_converter_new (GFile *base_file, GdkPixbufFormat *img_format, const char *format_str)
{
	EomURIConverter *conv;

	g_return_val_if_fail (format_str != NULL, NULL);

	conv = g_object_new (EOM_TYPE_URI_CONVERTER, NULL);

	if (base_file != NULL) {
		conv->priv->base_file  = g_object_ref (base_file);
	}
	else {
		conv->priv->base_file = NULL;
	}
	conv->priv->img_format = img_format;
	conv->priv->token_list = eom_uri_converter_parse_string (conv, format_str);

	return conv;
}

static GFile*
get_file_directory (EomURIConverter *conv, EomImage *image)
{
	GFile *file = NULL;
	EomURIConverterPrivate *priv;

	g_return_val_if_fail (EOM_IS_URI_CONVERTER (conv), NULL);
	g_return_val_if_fail (EOM_IS_IMAGE (image), NULL);

	priv = conv->priv;

	if (priv->base_file != NULL) {
		file = g_object_ref (priv->base_file);
	}
	else {
		GFile *img_file;

		img_file = eom_image_get_file (image);
		g_assert (img_file != NULL);

		file = g_file_get_parent (img_file);

		g_object_unref (img_file);
	}

	return file;
}

static void
split_filename (GFile *file, char **name, char **suffix)
{
	char *basename;
	char *suffix_start;
	guint len;

	*name = NULL;
	*suffix = NULL;

        /* get unescaped string */
	basename = g_file_get_basename (file);

	/* FIXME: does this work for all locales? */
	suffix_start = g_utf8_strrchr (basename, -1, '.');

	if (suffix_start == NULL) { /* no suffix found */
		*name = g_strdup (basename);
	}
	else {
		len = (suffix_start - basename);
		*name = g_strndup (basename, len);

		len = strlen (basename) - len - 1;
		*suffix = g_strndup (suffix_start+1, len);
	}

	g_free (basename);
}

static GString*
append_filename (GString *str, EomImage *img)
{
	/* appends the name of the original file without
	   filetype suffix */
	GFile *img_file;
	char *name;
	char *suffix;
	GString *result;

	img_file = eom_image_get_file (img);
	split_filename (img_file, &name, &suffix);

	result = g_string_append (str, name);

	g_free (name);
	g_free (suffix);

	g_object_unref (img_file);

	return result;
}

static GString*
append_counter (GString *str, gulong counter,  EomURIConverter *conv)
{
	EomURIConverterPrivate *priv;

	priv = conv->priv;

	g_string_append_printf (str, "%.*lu", priv->counter_n_digits, counter);

	return str;
}


static void
build_absolute_file (EomURIConverter *conv, EomImage *image, GString *str,  /* input  */
		     GFile **file, GdkPixbufFormat **format)                /* output */
{
	GFile *dir_file;
	EomURIConverterPrivate *priv;

	*file = NULL;
	if (format != NULL)
		*format = NULL;

	g_return_if_fail (EOM_IS_URI_CONVERTER (conv));
	g_return_if_fail (EOM_IS_IMAGE (image));
	g_return_if_fail (file != NULL);
	g_return_if_fail (str != NULL);

	priv = conv->priv;

	dir_file = get_file_directory (conv, image);
	g_assert (dir_file != NULL);

	if (priv->img_format == NULL) {
		/* use same file type/suffix */
		char *name;
		char *old_suffix;
		GFile *img_file;

		img_file = eom_image_get_file (image);
		split_filename (img_file, &name, &old_suffix);

		g_assert (old_suffix != NULL);

		g_string_append_unichar (str, '.');
		g_string_append (str, old_suffix);

		if (format != NULL)
			*format = eom_pixbuf_get_format_by_suffix (old_suffix);

		g_object_unref (img_file);
	} else {
		if (priv->suffix == NULL)
			priv->suffix = eom_pixbuf_get_common_suffix (priv->img_format);

		g_string_append_unichar (str, '.');
		g_string_append (str, priv->suffix);

		if (format != NULL)
			*format = priv->img_format;
	}

	*file = g_file_get_child (dir_file, str->str);

	g_object_unref (dir_file);
}


static GString*
replace_remove_chars (GString *str, gboolean convert_spaces, gunichar space_char)
{
	GString *result;
	guint len;
	char *s;
	int i;
	gunichar c;

	g_return_val_if_fail (str != NULL, NULL);

	if (!g_utf8_validate (str->str, -1, NULL))
	    return NULL;

	result = g_string_new (NULL);

	len = g_utf8_strlen (str->str, -1);
	s = str->str;

	for (i = 0; i < len; i++, s = g_utf8_next_char (s)) {
		c = g_utf8_get_char (s);

		if (c == '/') {
			continue;
		}
		else if (g_unichar_isspace (c) && convert_spaces) {
			result = g_string_append_unichar (result, space_char);
		}
		else {
			result = g_string_append_unichar (result, c);
		}
	}

	/* ensure maximum length of 250 characters */
	len = MIN (result->len, 250);
	result = g_string_truncate (result, len);

	return result;
}

/*
 * This function converts the uri of the EomImage object, according to the
 * EomUCToken list. The absolute uri (converted filename appended to base uri)
 * is returned in uri and the image format will be in the format pointer.
 */
gboolean
eom_uri_converter_do (EomURIConverter *conv, EomImage *image,
		      GFile **file, GdkPixbufFormat **format, GError **error)
{
	EomURIConverterPrivate *priv;
	GList *it;
	GString *str;
	GString *repl_str;

	g_return_val_if_fail (EOM_IS_URI_CONVERTER (conv), FALSE);

	priv = conv->priv;

	*file = NULL;
	if (format != NULL)
		*format = NULL;

	str = g_string_new ("");

	for (it = priv->token_list; it != NULL; it = it->next) {
		EomUCToken *token = (EomUCToken*) it->data;

		switch (token->type) {
		case EOM_UC_STRING:
			str = g_string_append (str, token->data.string);
			break;

		case EOM_UC_FILENAME:
			str = append_filename (str, image);
			break;

		case EOM_UC_COUNTER: {
			if (token->data.counter < priv->counter_start)
				token->data.counter = priv->counter_start;

			str = append_counter (str, token->data.counter++, conv);
			break;
		}
#if 0
		case EOM_UC_COMMENT:
			str = g_string_append_printf ();
			str = "comment";
			break;
		case EOM_UC_DATE:
			str = "date";
			break;
		case EOM_UC_TIME:
			str = "time";
			break;
		case EOM_UC_DAY:
			str = "day";
			break;
		case EOM_UC_MONTH:
			str = "month";
			break;
		case EOM_UC_YEAR:
			str = "year";
			break;
		case EOM_UC_HOUR:
			str = "hour";
			break;
		case EOM_UC_MINUTE:
			str = "minute";
			break;
		case EOM_UC_SECOND:
			str = "second";
			break;
#endif
		default:
		/* skip all others */

			break;
		}
	}

	repl_str = replace_remove_chars (str, priv->convert_spaces, priv->space_character);

	if (repl_str->len > 0) {
		build_absolute_file (conv, image, repl_str, file, format);
	}

	g_string_free (repl_str, TRUE);
	g_string_free (str, TRUE);


	return (*file != NULL);
}


char*
eom_uri_converter_preview (const char *format_str, EomImage *img, GdkPixbufFormat *format,
			   gulong counter, guint n_images,
			   gboolean convert_spaces, gunichar space_char)
{
	GString *str;
	GString *repl_str;
	guint n_digits;
	guint len;
	int i;
	const char *s;
	gunichar c;
	char *filename;
	gboolean token_next;

	g_return_val_if_fail (format_str != NULL, NULL);
	g_return_val_if_fail (EOM_IS_IMAGE (img), NULL);

	if (n_images == 0) return NULL;

	n_digits = ceil (MIN (log10 (G_MAXULONG), MAX (log10 (counter), log10 (n_images))));

	if (!g_utf8_validate (format_str, -1, NULL))
	    return NULL;

	len = g_utf8_strlen (format_str, -1);
	s = format_str;
	token_next = FALSE;

	str = g_string_new ("");

	for (i = 0; i < len; i++, s = g_utf8_next_char (s)) {
		c = g_utf8_get_char (s);

		if (token_next) {
			if (c == 'f') {
				str = append_filename (str, img);
			}
			else if (c == 'n') {
				g_string_append_printf (str, "%.*lu",
							n_digits ,counter);

			}
#if 0                   /* ignore the rest for now */
			else if (c == 'c') {
				type = EOM_UC_COMMENT;
			}
			else if (c == 'd') {
				type = EOM_UC_DATE;
			}
			else if (c == 't') {
				type = EOM_UC_TIME;
			}
			else if (c == 'a') {
				type = EOM_UC_DAY;
			}
			else if (c == 'm') {
				type = EOM_UC_MONTH;
			}
			else if (c == 'y') {
				type = EOM_UC_YEAR;
			}
			else if (c == 'h') {
				type = EOM_UC_HOUR;
			}
			else if (c == 'i') {
				type = EOM_UC_MINUTE;
			}
			else if (c == 's') {
				type = EOM_UC_SECOND;
			}
#endif
			token_next = FALSE;
		}
		else if (c == '%') {
			token_next = TRUE;
		}
		else {
			str = g_string_append_unichar (str, c);
		}
	}


	filename = NULL;
	repl_str = replace_remove_chars (str, convert_spaces, space_char);

	if (repl_str->len > 0) {
		if (format == NULL) {
			/* use same file type/suffix */
			char *name;
			char *old_suffix;
			GFile *img_file;

			img_file = eom_image_get_file (img);
			split_filename (img_file, &name, &old_suffix);

			g_assert (old_suffix != NULL);

			g_string_append_unichar (repl_str, '.');
			g_string_append (repl_str, old_suffix);

			g_free (old_suffix);
			g_free (name);
			g_object_unref (img_file);
		}
		else {
			char *suffix = eom_pixbuf_get_common_suffix (format);

			g_string_append_unichar (repl_str, '.');
			g_string_append (repl_str, suffix);

			g_free (suffix);
		}

		filename = repl_str->str;
	}

	g_string_free (repl_str, FALSE);
	g_string_free (str, TRUE);

	return filename;
}

gboolean
eom_uri_converter_requires_exif (EomURIConverter *converter)
{
	g_return_val_if_fail (EOM_IS_URI_CONVERTER (converter), FALSE);

	return converter->priv->requires_exif;
}

/**
 * eom_uri_converter_check:
 * @converter: a #EomURIConverter
 * @img_list: (element-type GFile): a #Gfile list
 * @error: a #GError location to store the error occurring, or NULL to ignore
 */

gboolean
eom_uri_converter_check (EomURIConverter *converter, GList *img_list, GError **error)
{
	GList *it;
	GList *file_list = NULL;
	gboolean all_different = TRUE;

	g_return_val_if_fail (EOM_IS_URI_CONVERTER (converter), FALSE);

	/* convert all image uris */
	for (it = img_list; it != NULL; it = it->next) {
		gboolean result;
		GFile *file;
		GError *conv_error = NULL;

		result = eom_uri_converter_do (converter, EOM_IMAGE (it->data),
					       &file, NULL, &conv_error);

		if (result) {
			file_list = g_list_prepend (file_list, file);
		}
	}

	/* check for all different uris */
	for (it = file_list; it != NULL && all_different; it = it->next) {
		GList *p;
		GFile *file;

		file = (GFile*) it->data;

		for (p = it->next; p != NULL && all_different; p = p->next) {
			all_different = !g_file_equal (file, (GFile*) p->data);
		}
	}

	if (!all_different) {
		g_set_error (error, EOM_UC_ERROR,
			     EOM_UC_ERROR_EQUAL_FILENAMES,
			     _("At least two file names are equal."));
	}

        g_list_free (file_list);

	return all_different;
}
