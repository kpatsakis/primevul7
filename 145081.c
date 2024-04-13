  PosibErr<void> Config::read_in(IStream & in, ParmStr id) 
  {
    String buf;
    DataPair dp;
    while (getdata_pair(in, dp, buf)) {
      to_lower(dp.key);
      Entry * entry = new Entry;
      entry->key = dp.key;
      entry->value = dp.value;
      entry->file = id;
      entry->line_num = dp.line_num;
      RET_ON_ERR(set(entry, true));
    }
    return no_err;
  }