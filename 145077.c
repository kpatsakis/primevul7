  void Config::set_filter_modules(const ConfigModule * modbegin, 
				  const ConfigModule * modend)
  {
    assert(filter_modules_ptrs.empty());
    filter_modules.clear();
    filter_modules.assign(modbegin, modend);
  }