    virtual const char * active_filter_module_desc(void) {
      if (m != 0)
        return m->desc;
      return "";
    }