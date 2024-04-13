  PosibErr<String> Config::get_default(ParmStr key) const
  {
    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);
    return get_default(ki);
  }