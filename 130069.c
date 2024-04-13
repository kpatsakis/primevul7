gboolean
mono_verifier_is_method_valid_generic_instantiation (MonoMethod *method)
{
	if (!method->is_inflated)
		return TRUE;
	return mono_method_is_valid_generic_instantiation (NULL, method);