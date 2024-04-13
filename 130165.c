mono_class_repect_method_constraints (VerifyContext *ctx, MonoClass *klass)
{
	MonoGenericClass *gklass = klass->generic_class;
	MonoGenericInst *ginst = gklass->context.class_inst;
	MonoGenericContainer *gc = gklass->container_class->generic_container;
	return !gc || generic_arguments_respect_constraints (ctx, gc, &gklass->context, ginst);
}