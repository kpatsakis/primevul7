  PosibErr<void> Config::merge(const Config & other)
  {
    const Entry * src  = other.first_;
    while (src) 
    {
      Entry * entry = new Entry(*src);
      entry->next = *insert_point_;
      *insert_point_ = entry;
      insert_point_ = &entry->next;
      if (committed_) RET_ON_ERR(commit(entry));
      src = src->next;
    }
    return no_err;
  }