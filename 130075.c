is_corlib_type (MonoClass *class)
{
	return class->image == mono_defaults.corlib;
}