  void Config::lang_config_merge(const Config & other,
                                 int which, ParmStr data_encoding)
  {
    Conv to_utf8;
    to_utf8.setup(*this, data_encoding, "utf-8", NormTo);
    const Entry * src  = other.first_;
    Entry * * ip = &first_;
    while (src)
    {
      const KeyInfo * l_ki = other.keyinfo(src->key);
      if (l_ki->other_data == which) {
        const KeyInfo * c_ki = keyinfo(src->key);
        Entry * entry = new Entry(*src);
        if (c_ki->flags & KEYINFO_UTF8)
          entry->value = to_utf8(entry->value);
        entry->next = *ip;
        *ip = entry;
        ip = &entry->next;
      }
      src = src->next;
    }
  }