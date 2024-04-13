mono_reflection_encode_sighelper (MonoDynamicImage *assembly, MonoReflectionSigHelper *helper)
{
	SigBuffer buf;
	guint32 nargs;
	guint32 size;
	guint32 i, idx;

	if (!assembly->save)
		return 0;

	/* FIXME: this means SignatureHelper.SignatureHelpType.HELPER_METHOD */
	g_assert (helper->type == 2);

	if (helper->arguments)
		nargs = mono_array_length (helper->arguments);
	else
		nargs = 0;

	size = 10 + (nargs * 10);
	
	sigbuffer_init (&buf, 32);

	/* Encode calling convention */
	/* Change Any to Standard */
	if ((helper->call_conv & 0x03) == 0x03)
		helper->call_conv = 0x01;
	/* explicit_this implies has_this */
	if (helper->call_conv & 0x40)
		helper->call_conv &= 0x20;

	if (helper->call_conv == 0) { /* Unmanaged */
		idx = helper->unmanaged_call_conv - 1;
	} else {
		/* Managed */
		idx = helper->call_conv & 0x60; /* has_this + explicit_this */
		if (helper->call_conv & 0x02) /* varargs */
			idx += 0x05;
	}

	sigbuffer_add_byte (&buf, idx);
	sigbuffer_add_value (&buf, nargs);
	encode_reflection_type (assembly, helper->return_type, &buf);
	for (i = 0; i < nargs; ++i) {
		MonoArray *modreqs = NULL;
		MonoArray *modopts = NULL;
		MonoReflectionType *pt;

		if (helper->modreqs && (i < mono_array_length (helper->modreqs)))
			modreqs = mono_array_get (helper->modreqs, MonoArray*, i);
		if (helper->modopts && (i < mono_array_length (helper->modopts)))
			modopts = mono_array_get (helper->modopts, MonoArray*, i);

		encode_custom_modifiers (assembly, modreqs, modopts, &buf);
		pt = mono_array_get (helper->arguments, MonoReflectionType*, i);
		encode_reflection_type (assembly, pt, &buf);
	}
	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);

	return idx;
}