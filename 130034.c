verifier_get_generic_param_from_type (VerifyContext *ctx, MonoType *type)
{
	MonoGenericContainer *gc;
	MonoMethod *method = ctx->method;
	int num;

	num = mono_type_get_generic_param_num (type);

	if (type->type == MONO_TYPE_VAR) {
		MonoClass *gtd = method->klass;
		if (gtd->generic_class)
			gtd = gtd->generic_class->container_class;
		gc = gtd->generic_container;
	} else { //MVAR
		MonoMethod *gmd = method;
		if (method->is_inflated)
			gmd = ((MonoMethodInflated*)method)->declaring;
		gc = mono_method_get_generic_container (gmd);
	}
	if (!gc)
		return FALSE;
	return mono_generic_container_get_param (gc, num);
}