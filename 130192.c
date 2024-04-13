mono_delegate_signature_equal (MonoMethodSignature *delegate_sig, MonoMethodSignature *method_sig, gboolean is_static_ldftn)
{
	int i;
	int method_offset = is_static_ldftn ? 1 : 0;

	if (delegate_sig->param_count + method_offset != method_sig->param_count) 
		return FALSE;

	if (delegate_sig->call_convention != method_sig->call_convention)
		return FALSE;

	for (i = 0; i < delegate_sig->param_count; i++) { 
		MonoType *p1 = delegate_sig->params [i];
		MonoType *p2 = method_sig->params [i + method_offset];

		if (!mono_delegate_param_equal (p1, p2))
			return FALSE;
	}

	if (!mono_delegate_ret_equal (delegate_sig->ret, method_sig->ret))
		return FALSE;

	return TRUE;
}