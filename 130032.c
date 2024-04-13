mono_class_get_ref_info (MonoClass *klass)
{
	if (klass->ref_info_handle == 0)
		return NULL;
	else
		return mono_gchandle_get_target (klass->ref_info_handle);
}