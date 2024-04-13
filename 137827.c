static PHP_GINIT_FUNCTION(spl)
{
	spl_globals->autoload_extensions     = NULL;
	spl_globals->autoload_extensions_len = 0;
	spl_globals->autoload_functions      = NULL;
	spl_globals->autoload_running        = 0;
}