mono_image_basic_init (MonoReflectionAssemblyBuilder *assemblyb)
{
	MonoDynamicAssembly *assembly;
	MonoDynamicImage *image;
	MonoDomain *domain = mono_object_domain (assemblyb);
	
	MONO_ARCH_SAVE_REGS;

	if (assemblyb->dynamic_assembly)
		return;

#if HAVE_BOEHM_GC
	/* assembly->assembly.image might be GC allocated */
	assembly = assemblyb->dynamic_assembly = GC_MALLOC (sizeof (MonoDynamicAssembly));
#else
	assembly = assemblyb->dynamic_assembly = g_new0 (MonoDynamicAssembly, 1);
#endif

	mono_profiler_assembly_event (&assembly->assembly, MONO_PROFILE_START_LOAD);
	
	assembly->assembly.ref_count = 1;
	assembly->assembly.dynamic = TRUE;
	assembly->assembly.corlib_internal = assemblyb->corlib_internal;
	assemblyb->assembly.assembly = (MonoAssembly*)assembly;
	assembly->assembly.basedir = mono_string_to_utf8 (assemblyb->dir);
	if (assemblyb->culture)
		assembly->assembly.aname.culture = mono_string_to_utf8 (assemblyb->culture);
	else
		assembly->assembly.aname.culture = g_strdup ("");

        if (assemblyb->version) {
			char *vstr = mono_string_to_utf8 (assemblyb->version);
			char **version = g_strsplit (vstr, ".", 4);
			char **parts = version;
			assembly->assembly.aname.major = atoi (*parts++);
			assembly->assembly.aname.minor = atoi (*parts++);
			assembly->assembly.aname.build = *parts != NULL ? atoi (*parts++) : 0;
			assembly->assembly.aname.revision = *parts != NULL ? atoi (*parts) : 0;

			g_strfreev (version);
			g_free (vstr);
        } else {
			assembly->assembly.aname.major = 0;
			assembly->assembly.aname.minor = 0;
			assembly->assembly.aname.build = 0;
			assembly->assembly.aname.revision = 0;
        }

	assembly->run = assemblyb->access != 2;
	assembly->save = assemblyb->access != 1;
	assembly->domain = domain;

	image = create_dynamic_mono_image (assembly, mono_string_to_utf8 (assemblyb->name), g_strdup ("RefEmit_YouForgotToDefineAModule"));
	image->initial_image = TRUE;
	assembly->assembly.aname.name = image->image.name;
	assembly->assembly.image = &image->image;
	if (assemblyb->pktoken && assemblyb->pktoken->max_length) {
		/* -1 to correct for the trailing NULL byte */
		if (assemblyb->pktoken->max_length != MONO_PUBLIC_KEY_TOKEN_LENGTH - 1) {
			g_error ("Public key token length invalid for assembly %s: %i", assembly->assembly.aname.name, assemblyb->pktoken->max_length);
		}
		memcpy (&assembly->assembly.aname.public_key_token, mono_array_addr (assemblyb->pktoken, guint8, 0), assemblyb->pktoken->max_length);		
	}

	mono_domain_assemblies_lock (domain);
	domain->domain_assemblies = g_slist_prepend (domain->domain_assemblies, assembly);
	mono_domain_assemblies_unlock (domain);

	register_assembly (mono_object_domain (assemblyb), &assemblyb->assembly, &assembly->assembly);
	
	mono_profiler_assembly_loaded (&assembly->assembly, MONO_PROFILE_OK);
	
	mono_assembly_invoke_load_hook ((MonoAssembly*)assembly);
}