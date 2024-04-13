mono_type_is_valid_type_in_context (MonoType *type, MonoGenericContext *context)
{
	int i;
	MonoGenericInst *inst;

	switch (type->type) {
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
		if (!context)
			return FALSE;
		inst = type->type == MONO_TYPE_VAR ? context->class_inst : context->method_inst;
		if (!inst || mono_type_get_generic_param_num (type) >= inst->type_argc)
			return FALSE;
		break;
	case MONO_TYPE_SZARRAY:
		return mono_type_is_valid_type_in_context (&type->data.klass->byval_arg, context);
	case MONO_TYPE_ARRAY:
		return mono_type_is_valid_type_in_context (&type->data.array->eklass->byval_arg, context);
	case MONO_TYPE_PTR:
		return mono_type_is_valid_type_in_context (type->data.type, context);
	case MONO_TYPE_GENERICINST:
		inst = type->data.generic_class->context.class_inst;
		if (!inst->is_open)
			break;
		for (i = 0; i < inst->type_argc; ++i)
			if (!mono_type_is_valid_type_in_context (inst->type_argv [i], context))
				return FALSE;
		break;
	}
	return TRUE;
}