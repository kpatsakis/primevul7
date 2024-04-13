MonoReflectionMarshal*
mono_reflection_marshal_from_marshal_spec (MonoDomain *domain, MonoClass *klass,
										   MonoMarshalSpec *spec)
{
	static MonoClass *System_Reflection_Emit_UnmanagedMarshalClass;
	MonoReflectionMarshal *minfo;
	MonoType *mtype;

	if (!System_Reflection_Emit_UnmanagedMarshalClass) {
		System_Reflection_Emit_UnmanagedMarshalClass = mono_class_from_name (
		   mono_defaults.corlib, "System.Reflection.Emit", "UnmanagedMarshal");
		g_assert (System_Reflection_Emit_UnmanagedMarshalClass);
	}

	minfo = (MonoReflectionMarshal*)mono_object_new (domain, System_Reflection_Emit_UnmanagedMarshalClass);
	minfo->type = spec->native;

	switch (minfo->type) {
	case MONO_NATIVE_LPARRAY:
		minfo->eltype = spec->data.array_data.elem_type;
		minfo->count = spec->data.array_data.num_elem;
		minfo->param_num = spec->data.array_data.param_num;
		break;

	case MONO_NATIVE_BYVALTSTR:
	case MONO_NATIVE_BYVALARRAY:
		minfo->count = spec->data.array_data.num_elem;
		break;

	case MONO_NATIVE_CUSTOM:
		if (spec->data.custom_data.custom_name) {
			mtype = mono_reflection_type_from_name (spec->data.custom_data.custom_name, klass->image);
			if (mtype)
				MONO_OBJECT_SETREF (minfo, marshaltyperef, mono_type_get_object (domain, mtype));

			MONO_OBJECT_SETREF (minfo, marshaltype, mono_string_new (domain, spec->data.custom_data.custom_name));
		}
		if (spec->data.custom_data.cookie)
			MONO_OBJECT_SETREF (minfo, mcookie, mono_string_new (domain, spec->data.custom_data.cookie));
		break;

	default:
		break;
	}

	return minfo;