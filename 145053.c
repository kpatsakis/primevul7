  void Config::del()
  {
    while (first_) {
      Entry * tmp = first_->next;
      delete first_;
      first_ = tmp;
    }

    while (others_) {
      Entry * tmp = others_->next;
      delete first_;
      others_ = tmp;
    }

    Vector<Notifier *>::iterator i   = notifier_list.begin();
    Vector<Notifier *>::iterator end = notifier_list.end();

    for(; i != end; ++i) {
      delete (*i);
      *i = 0;
    }
    
    notifier_list.clear();

#ifdef HAVE_LIBDL
    filter_modules.clear();
    for (Vector<Cacheable *>::iterator i = filter_modules_ptrs.begin();
         i != filter_modules_ptrs.end();
         ++i)
      (*i)->release();
    filter_modules_ptrs.clear();
#endif
  }