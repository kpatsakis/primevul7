mono_class_set_ref_info (MonoClass *klass, gpointer obj)
{
	klass->ref_info_handle = mono_gchandle_new ((MonoObject*)obj, FALSE);
	g_assert (klass->ref_info_handle != 0);
}