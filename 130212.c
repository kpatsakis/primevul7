void
mono_free_verify_list (GSList *list)
{
	/* The verifier was disabled at compile time */
	/* will always be null if verifier is disabled */