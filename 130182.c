gboolean
mono_verifier_is_class_full_trust (MonoClass *klass)
{
	/* The verifier was disabled at compile time */
	return TRUE;