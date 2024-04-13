verify_safe_for_managed_space (MonoType *type)
{
	switch (type->type) {
#ifdef DEBUG_HARDER
	case MONO_TYPE_ARRAY:
		return verify_safe_for_managed_space (&type->data.array->eklass->byval_arg);
	case MONO_TYPE_PTR:
		return verify_safe_for_managed_space (type->data.type);
	case MONO_TYPE_SZARRAY:
		return verify_safe_for_managed_space (&type->data.klass->byval_arg);
	case MONO_TYPE_GENERICINST: {
		MonoGenericInst *inst = type->data.generic_class->inst;
		int i;
		if (!inst->is_open)
			break;
		for (i = 0; i < inst->type_argc; ++i)
			if (!verify_safe_for_managed_space (inst->type_argv [i]))
				return FALSE;
		break;
	}
#endif
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
		return TRUE;
	}
	return TRUE;
}