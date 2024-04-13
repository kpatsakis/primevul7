  void Config::copy(const Config & other)
  {
    assert(other.others_ == 0);
    others_ = 0;

    name_ = other.name_;

    committed_ = other.committed_;
    attached_ = other.attached_;
    settings_read_in_ = other.settings_read_in_;

    keyinfo_begin = other.keyinfo_begin;
    keyinfo_end   = other.keyinfo_end;
    extra_begin   = other.extra_begin;
    extra_end     = other.extra_end;
    filter_modules = other.filter_modules;

#ifdef HAVE_LIBDL
    filter_modules_ptrs = other.filter_modules_ptrs;
    for (Vector<Cacheable *>::iterator i = filter_modules_ptrs.begin();
         i != filter_modules_ptrs.end();
         ++i)
      (*i)->copy();
#endif

    md_info_list_index = other.md_info_list_index;

    insert_point_ = 0;
    Entry * const * src  = &other.first_;
    Entry * * dest = &first_;
    while (*src) 
    {
      *dest = new Entry(**src);
      if (src == other.insert_point_)
        insert_point_ = dest;
      src  = &((*src)->next);
      dest = &((*dest)->next);
    }
    if (insert_point_ == 0)
      insert_point_ = dest;
    *dest = 0;

    Vector<Notifier *>::const_iterator i   = other.notifier_list.begin();
    Vector<Notifier *>::const_iterator end = other.notifier_list.end();

    for(; i != end; ++i) {
      Notifier * tmp = (*i)->clone(this);
      if (tmp != 0)
	notifier_list.push_back(tmp);
    }
  }