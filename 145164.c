static void core_globals_ctor(php_core_globals *core_globals)
{
	memset(core_globals, 0, sizeof(*core_globals));
}