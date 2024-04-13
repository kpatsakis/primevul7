mono_method_repect_method_constraints (VerifyContext *ctx, MonoMethod *method)
{
	MonoMethodInflated *gmethod = (MonoMethodInflated *)method;
	MonoGenericInst *ginst = gmethod->context.method_inst;
	MonoGenericContainer *gc = mono_method_get_generic_container (gmethod->declaring);
	return !gc || generic_arguments_respect_constraints (ctx, gc, &gmethod->context, ginst);
}