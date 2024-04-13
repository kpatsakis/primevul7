  PosibErr<void> Config::remove(ParmStr key)
  {
    Entry * entry = new Entry;
    entry->key = key;
    entry->action = Reset;
    return set(entry);
  }