  PosibErr<bool> ListAddHelper::add(ParmStr val)
  {
    Config::Entry * entry = new Config::Entry(*orig_entry);
    entry->value = val;
    entry->action = Config::ListAdd;
    config->set(entry);
    return true;
  }