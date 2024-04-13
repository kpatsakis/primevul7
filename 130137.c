mono_reflection_method_on_tb_inst_get_handle (MonoReflectionMethodOnTypeBuilderInst *m)
{
	MonoClass *klass;
	MonoGenericContext tmp_context;
	MonoType **type_argv;
	MonoGenericInst *ginst;
	MonoMethod *method, *inflated;
	int count, i;

	method = inflate_method (m->inst, (MonoObject*)m->mb);

	klass = method->klass;

	if (m->method_args == NULL)
		return method;

	if (method->is_inflated)
		method = ((MonoMethodInflated *) method)->declaring;

	count = mono_array_length (m->method_args);

	type_argv = g_new0 (MonoType *, count);
	for (i = 0; i < count; i++) {
		MonoReflectionType *garg = mono_array_get (m->method_args, gpointer, i);
		type_argv [i] = mono_reflection_type_get_handle (garg);
	}
	ginst = mono_metadata_get_generic_inst (count, type_argv);
	g_free (type_argv);

	tmp_context.class_inst = klass->generic_class ? klass->generic_class->context.class_inst : NULL;
	tmp_context.method_inst = ginst;

	inflated = mono_class_inflate_generic_method (method, &tmp_context);
	return inflated;
}