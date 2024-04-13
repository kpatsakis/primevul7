static MonoMethod *
inflate_mono_method (MonoClass *klass, MonoMethod *method, MonoObject *obj)
{
	MonoMethodInflated *imethod;
	MonoGenericContext *context;
	int i;

	/*
	 * With generic code sharing the klass might not be inflated.
	 * This can happen because classes inflated with their own
	 * type arguments are "normalized" to the uninflated class.
	 */
	if (!klass->generic_class)
		return method;

	context = mono_class_get_context (klass);

	if (klass->method.count && klass->methods) {
		/* Find the already created inflated method */
		for (i = 0; i < klass->method.count; ++i) {
			g_assert (klass->methods [i]->is_inflated);
			if (((MonoMethodInflated*)klass->methods [i])->declaring == method)
				break;
		}
		g_assert (i < klass->method.count);
		imethod = (MonoMethodInflated*)klass->methods [i];
	} else {
		imethod = (MonoMethodInflated *) mono_class_inflate_generic_method_full (method, klass, context);
	}

	if (method->is_generic && method->klass->image->dynamic) {
		MonoDynamicImage *image = (MonoDynamicImage*)method->klass->image;

		mono_loader_lock ();
		mono_g_hash_table_insert (image->generic_def_objects, imethod, obj);
		mono_loader_unlock ();
	}
	return (MonoMethod *) imethod;