compare_constants (const void *a, const void *b)
{
	const guint32 *a_values = a;
	const guint32 *b_values = b;
	return a_values [MONO_CONSTANT_PARENT] - b_values [MONO_CONSTANT_PARENT];
}