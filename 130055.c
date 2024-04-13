void
mono_image_basic_init (MonoReflectionAssemblyBuilder *assemblyb)
{
	g_error ("This mono runtime was configured with --enable-minimal=reflection_emit, so System.Reflection.Emit is not supported.");