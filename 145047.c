  PosibErr<void> Config::retrieve_list(ParmStr key, 
				       MutableContainer * m) const
  {
    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);
    if (ki->type != KeyInfoList) return make_err(key_not_list, ki->name);

    lookup_list(ki, *m, true);

    return no_err;
  }