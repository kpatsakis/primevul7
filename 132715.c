plugin_parse_file (Ekiga::KickStart& kickstart,
		   const gchar* filename)
{
#if DEBUG
  std::cout << "Trying to load " << filename << "... ";
#endif
  GModule* plugin = g_module_open (filename, G_MODULE_BIND_LOCAL);

  if (plugin != 0) {

#if DEBUG
    std::cout << "loaded... ";
#endif
    gpointer init_func = NULL;

    if (g_module_symbol (plugin, "ekiga_plugin_init", &init_func)) {

#if DEBUG
      std::cout << "valid" << std::endl;
#endif
      g_module_make_resident (plugin);
      ((void (*)(Ekiga::KickStart&))init_func) (kickstart);
    } else {

#if DEBUG
      std::cout << "invalid: " << g_module_error () << std::endl;
#endif
      g_module_close (plugin);
    }
  } else {

#if DEBUG
    std::cout << "failed to load the module: " << g_module_error () << std::endl;
#endif
  }
}