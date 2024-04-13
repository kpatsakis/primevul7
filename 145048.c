  void Config::lookup_list(const KeyInfo * ki,
                           MutableContainer & m,
                           bool include_default) const
  {
    const Entry * cur = first_;
    const Entry * first_to_use = 0;

    while (cur) {
      if (cur->key == ki->name && 
          (first_to_use == 0 || 
           cur->action == Reset || cur->action == Set 
           || cur->action == ListClear)) 
        first_to_use = cur;
      cur = cur->next;
    }

    cur = first_to_use;

    if (include_default && 
        (!cur || 
         !(cur->action == Set || cur->action == ListClear)))
    {
      String def = get_default(ki);
      separate_list(def, m, true);
    }

    if (cur && cur->action == Reset) {
      cur = cur->next;
    }

    if (cur && cur->action == Set) {
      if (!include_default) m.clear();
      m.add(cur->value);
      cur = cur->next;
    }

    if (cur && cur->action == ListClear) {
      if (!include_default) m.clear();
      cur = cur->next;
    }

    while (cur) {
      if (cur->key == ki->name) {
        if (cur->action == ListAdd)
          m.add(cur->value);
        else if (cur->action == ListRemove)
          m.remove(cur->value);
      }
      cur = cur->next;
    }
  }