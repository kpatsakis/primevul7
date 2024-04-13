mono_param_get_objects_internal (MonoDomain *domain, MonoMethod *method, MonoClass *refclass)
{
	static MonoClass *System_Reflection_ParameterInfo;
	static MonoClass *System_Reflection_ParameterInfo_array;
	MonoArray *res = NULL;
	MonoReflectionMethod *member = NULL;
	MonoReflectionParameter *param = NULL;
	char **names, **blobs = NULL;
	guint32 *types = NULL;
	MonoType *type = NULL;
	MonoObject *dbnull = NULL;
	MonoObject *missing = NULL;
	MonoMarshalSpec **mspecs;
	MonoMethodSignature *sig;
	MonoVTable *pinfo_vtable;
	int i;

	if (!System_Reflection_ParameterInfo_array) {
		MonoClass *klass;

		klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "ParameterInfo");
		mono_memory_barrier ();
		System_Reflection_ParameterInfo = klass; 
	
		klass = mono_array_class_get (klass, 1);
		mono_memory_barrier ();
		System_Reflection_ParameterInfo_array = klass;
	}
	
	if (!mono_method_signature (method)->param_count)
		return mono_array_new_specific (mono_class_vtable (domain, System_Reflection_ParameterInfo_array), 0);

	/* Note: the cache is based on the address of the signature into the method
	 * since we already cache MethodInfos with the method as keys.
	 */
	CHECK_OBJECT (MonoArray*, &(method->signature), refclass);

	sig = mono_method_signature (method);
	member = mono_method_get_object (domain, method, refclass);
	names = g_new (char *, sig->param_count);
	mono_method_get_param_names (method, (const char **) names);

	mspecs = g_new (MonoMarshalSpec*, sig->param_count + 1);
	mono_method_get_marshal_info (method, mspecs);

	res = mono_array_new_specific (mono_class_vtable (domain, System_Reflection_ParameterInfo_array), sig->param_count);
	pinfo_vtable = mono_class_vtable (domain, System_Reflection_ParameterInfo);
	for (i = 0; i < sig->param_count; ++i) {
		param = (MonoReflectionParameter *)mono_object_new_specific (pinfo_vtable);
		MONO_OBJECT_SETREF (param, ClassImpl, mono_type_get_object (domain, sig->params [i]));
		MONO_OBJECT_SETREF (param, MemberImpl, (MonoObject*)member);
		MONO_OBJECT_SETREF (param, NameImpl, mono_string_new (domain, names [i]));
		param->PositionImpl = i;
		param->AttrsImpl = sig->params [i]->attrs;

		if (!(param->AttrsImpl & PARAM_ATTRIBUTE_HAS_DEFAULT)) {
			if (param->AttrsImpl & PARAM_ATTRIBUTE_OPTIONAL)
				MONO_OBJECT_SETREF (param, DefaultValueImpl, get_reflection_missing (domain, &missing));
			else
				MONO_OBJECT_SETREF (param, DefaultValueImpl, get_dbnull (domain, &dbnull));
		} else {

			if (!blobs) {
				blobs = g_new0 (char *, sig->param_count);
				types = g_new0 (guint32, sig->param_count);
				get_default_param_value_blobs (method, blobs, types); 
			}

			/* Build MonoType for the type from the Constant Table */
			if (!type)
				type = g_new0 (MonoType, 1);
			type->type = types [i];
			type->data.klass = NULL;
			if (types [i] == MONO_TYPE_CLASS)
				type->data.klass = mono_defaults.object_class;
			else if ((sig->params [i]->type == MONO_TYPE_VALUETYPE) && sig->params [i]->data.klass->enumtype) {
				/* For enums, types [i] contains the base type */

					type->type = MONO_TYPE_VALUETYPE;
					type->data.klass = mono_class_from_mono_type (sig->params [i]);
			} else
				type->data.klass = mono_class_from_mono_type (type);

			MONO_OBJECT_SETREF (param, DefaultValueImpl, mono_get_object_from_blob (domain, type, blobs [i]));

			/* Type in the Constant table is MONO_TYPE_CLASS for nulls */
			if (types [i] != MONO_TYPE_CLASS && !param->DefaultValueImpl) {
				if (param->AttrsImpl & PARAM_ATTRIBUTE_OPTIONAL)
					MONO_OBJECT_SETREF (param, DefaultValueImpl, get_reflection_missing (domain, &missing));
				else
					MONO_OBJECT_SETREF (param, DefaultValueImpl, get_dbnull (domain, &dbnull));
			}
			
		}

		if (mspecs [i + 1])
			MONO_OBJECT_SETREF (param, MarshalAsImpl, (MonoObject*)mono_reflection_marshal_from_marshal_spec (domain, method->klass, mspecs [i + 1]));
		
		mono_array_setref (res, i, param);
	}
	g_free (names);
	g_free (blobs);
	g_free (types);
	g_free (type);

	for (i = mono_method_signature (method)->param_count; i >= 0; i--)
		if (mspecs [i])
			mono_metadata_free_marshal_spec (mspecs [i]);
	g_free (mspecs);
	
	CACHE_OBJECT (MonoArray *, &(method->signature), res, refclass);
}