encode_type (MonoDynamicImage *assembly, MonoType *type, SigBuffer *buf)
{
	if (!type) {
		g_assert_not_reached ();
		return;
	}
		
	if (type->byref)
		sigbuffer_add_value (buf, MONO_TYPE_BYREF);

	switch (type->type){
	case MONO_TYPE_VOID:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
	case MONO_TYPE_I:
	case MONO_TYPE_U:
	case MONO_TYPE_STRING:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_TYPEDBYREF:
		sigbuffer_add_value (buf, type->type);
		break;
	case MONO_TYPE_PTR:
		sigbuffer_add_value (buf, type->type);
		encode_type (assembly, type->data.type, buf);
		break;
	case MONO_TYPE_SZARRAY:
		sigbuffer_add_value (buf, type->type);
		encode_type (assembly, &type->data.klass->byval_arg, buf);
		break;
	case MONO_TYPE_VALUETYPE:
	case MONO_TYPE_CLASS: {
		MonoClass *k = mono_class_from_mono_type (type);

		if (k->generic_container) {
			MonoGenericClass *gclass = mono_metadata_lookup_generic_class (k, k->generic_container->context.class_inst, TRUE);
			encode_generic_class (assembly, gclass, buf);
		} else {
			/*
			 * Make sure we use the correct type.
			 */
			sigbuffer_add_value (buf, k->byval_arg.type);
			/*
			 * ensure only non-byref gets passed to mono_image_typedef_or_ref(),
			 * otherwise two typerefs could point to the same type, leading to
			 * verification errors.
			 */
			sigbuffer_add_value (buf, mono_image_typedef_or_ref (assembly, &k->byval_arg));
		}
		break;
	}
	case MONO_TYPE_ARRAY:
		sigbuffer_add_value (buf, type->type);
		encode_type (assembly, &type->data.array->eklass->byval_arg, buf);
		sigbuffer_add_value (buf, type->data.array->rank);
		sigbuffer_add_value (buf, 0); /* FIXME: set to 0 for now */
		sigbuffer_add_value (buf, 0);
		break;
	case MONO_TYPE_GENERICINST:
		encode_generic_class (assembly, type->data.generic_class, buf);
		break;
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
		sigbuffer_add_value (buf, type->type);
		sigbuffer_add_value (buf, mono_type_get_generic_param_num (type));
		break;
	default:
		g_error ("need to encode type %x", type->type);
	}
}