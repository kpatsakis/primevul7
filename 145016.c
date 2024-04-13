  Config * new_basic_config() { 
    aspell_gettext_init();
    return new Config("aspell",
		      config_impl_keys_begin,
		      config_impl_keys_end);
  }