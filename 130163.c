mono_reflection_create_custom_attr_data_args (MonoImage *image, MonoMethod *method, const guchar *data, guint32 len, MonoArray **typed_args, MonoArray **named_args, CattrNamedArg **named_arg_info)
{
	MonoArray *typedargs, *namedargs;
	MonoClass *attrklass;
	MonoDomain *domain;
	const char *p = (const char*)data;
	const char *named;
	guint32 i, j, num_named;
	CattrNamedArg *arginfo = NULL;

	if (!mono_verifier_verify_cattr_content (image, method, data, len, NULL))
		return;

	mono_class_init (method->klass);

	*typed_args = NULL;
	*named_args = NULL;
	*named_arg_info = NULL;
	
	domain = mono_domain_get ();

	if (len < 2 || read16 (p) != 0x0001) /* Prolog */
		return;

	typedargs = mono_array_new (domain, mono_get_object_class (), mono_method_signature (method)->param_count);
	
	/* skip prolog */
	p += 2;
	for (i = 0; i < mono_method_signature (method)->param_count; ++i) {
		MonoObject *obj;
		void *val;

		val = load_cattr_value (image, mono_method_signature (method)->params [i], p, &p);
		obj = type_is_reference (mono_method_signature (method)->params [i]) ? 
			val : mono_value_box (domain, mono_class_from_mono_type (mono_method_signature (method)->params [i]), val);
		mono_array_setref (typedargs, i, obj);

		if (!type_is_reference (mono_method_signature (method)->params [i]))
			g_free (val);
	}

	named = p;
	num_named = read16 (named);
	namedargs = mono_array_new (domain, mono_get_object_class (), num_named);
	named += 2;
	attrklass = method->klass;

	arginfo = g_new0 (CattrNamedArg, num_named);
	*named_arg_info = arginfo;

	for (j = 0; j < num_named; j++) {
		gint name_len;
		char *name, named_type, data_type;
		named_type = *named++;
		data_type = *named++; /* type of data */
		if (data_type == MONO_TYPE_SZARRAY)
			data_type = *named++;
		if (data_type == MONO_TYPE_ENUM) {
			gint type_len;
			char *type_name;
			type_len = mono_metadata_decode_blob_size (named, &named);
			type_name = g_malloc (type_len + 1);
			memcpy (type_name, named, type_len);
			type_name [type_len] = 0;
			named += type_len;
			/* FIXME: lookup the type and check type consistency */
			g_free (type_name);
		}
		name_len = mono_metadata_decode_blob_size (named, &named);
		name = g_malloc (name_len + 1);
		memcpy (name, named, name_len);
		name [name_len] = 0;
		named += name_len;
		if (named_type == 0x53) {
			MonoObject *obj;
			MonoClassField *field = mono_class_get_field_from_name (attrklass, name);
			void *val;

			arginfo [j].type = field->type;
			arginfo [j].field = field;

			val = load_cattr_value (image, field->type, named, &named);
			obj = type_is_reference (field->type) ? val : mono_value_box (domain, mono_class_from_mono_type (field->type), val);
			mono_array_setref (namedargs, j, obj);
			if (!type_is_reference (field->type))
				g_free (val);
		} else if (named_type == 0x54) {
			MonoObject *obj;
			MonoType *prop_type;
			MonoProperty *prop = mono_class_get_property_from_name (attrklass, name);
			void *val;

			prop_type = prop->get? mono_method_signature (prop->get)->ret :
			     mono_method_signature (prop->set)->params [mono_method_signature (prop->set)->param_count - 1];

			arginfo [j].type = prop_type;
			arginfo [j].prop = prop;

			val = load_cattr_value (image, prop_type, named, &named);
			obj = type_is_reference (prop_type) ? val : mono_value_box (domain, mono_class_from_mono_type (prop_type), val);
			mono_array_setref (namedargs, j, obj);
			if (!type_is_reference (prop_type))
				g_free (val);
		}
		g_free (name);
	}

	*typed_args = typedargs;
	*named_args = namedargs;
}