gboolean
mono_reflection_call_is_assignable_to (MonoClass *klass, MonoClass *oklass)
{
	MonoObject *res, *exc;
	void *params [1];
	static MonoClass *System_Reflection_Emit_TypeBuilder = NULL;
	static MonoMethod *method = NULL;

	if (!System_Reflection_Emit_TypeBuilder) {
		System_Reflection_Emit_TypeBuilder = mono_class_from_name (mono_defaults.corlib, "System.Reflection.Emit", "TypeBuilder");
		g_assert (System_Reflection_Emit_TypeBuilder);
	}
	if (method == NULL) {
		method = mono_class_get_method_from_name (System_Reflection_Emit_TypeBuilder, "IsAssignableTo", 1);
		g_assert (method);
	}

	/* 
	 * The result of mono_type_get_object () might be a System.MonoType but we
	 * need a TypeBuilder so use mono_class_get_ref_info (klass).
	 */
	g_assert (mono_class_get_ref_info (klass));
	g_assert (!strcmp (((MonoObject*)(mono_class_get_ref_info (klass)))->vtable->klass->name, "TypeBuilder"));

	params [0] = mono_type_get_object (mono_domain_get (), &oklass->byval_arg);

	res = mono_runtime_invoke (method, (MonoObject*)(mono_class_get_ref_info (klass)), params, &exc);
	if (exc)
		return FALSE;
	else
		return *(MonoBoolean*)mono_object_unbox (res);