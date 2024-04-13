gboolean
mono_verifier_is_method_full_trust (MonoMethod *method)
{
	/* The verifier was disabled at compile time */
	return TRUE;