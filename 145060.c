  PosibErr<void> Config::replace(ParmStr key, ParmStr value)
  {
    Entry * entry = new Entry;
    entry->key = key;
    entry->value = value;
    return set(entry);
  }