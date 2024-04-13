    const char * active_filter_module_name(void){
      if (m != 0)
        return m->name;
      return "";
    }