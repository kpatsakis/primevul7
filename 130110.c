gboolean
mono_class_is_reflection_method_or_constructor (MonoClass *class)
{
	return is_sr_mono_method (class) || is_sr_mono_cmethod (class) || is_sr_mono_generic_method (class) || is_sr_mono_generic_cmethod (class);