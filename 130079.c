mono_delegate_param_equal (MonoType *delegate, MonoType *method)
{
	if (mono_metadata_type_equal_full (delegate, method, TRUE))
		return TRUE;

	return mono_delegate_type_equal (method, delegate);
}