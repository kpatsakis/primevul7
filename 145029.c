  bool Config::have(ParmStr key) const 
  {
    PosibErr<const KeyInfo *> pe = keyinfo(key);
    if (pe.has_err()) {pe.ignore_err(); return false;}
    return lookup(pe.data->name);
  }