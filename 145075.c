  void Config::replace_internal(ParmStr key, ParmStr value)
  {
    Entry * entry = new Entry;
    entry->key = key;
    entry->value = value;
    entry->action = Set;
    entry->next = *insert_point_;
    *insert_point_ = entry;
    insert_point_ = &entry->next;
  }