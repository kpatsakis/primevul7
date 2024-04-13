method_encode_signature (MonoDynamicImage *assembly, MonoMethodSignature *sig)
{
	SigBuffer buf;
	int i;
	guint32 nparams =  sig->param_count;
	guint32 idx;

	if (!assembly->save)
		return 0;

	sigbuffer_init (&buf, 32);
	/*
	 * FIXME: vararg, explicit_this, differenc call_conv values...
	 */
	idx = sig->call_convention;
	if (sig->hasthis)
		idx |= 0x20; /* hasthis */
	if (sig->generic_param_count)
		idx |= 0x10; /* generic */
	sigbuffer_add_byte (&buf, idx);
	if (sig->generic_param_count)
		sigbuffer_add_value (&buf, sig->generic_param_count);
	sigbuffer_add_value (&buf, nparams);
	encode_type (assembly, sig->ret, &buf);
	for (i = 0; i < nparams; ++i) {
		if (i == sig->sentinelpos)
			sigbuffer_add_byte (&buf, MONO_TYPE_SENTINEL);
		encode_type (assembly, sig->params [i], &buf);
	}
	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}