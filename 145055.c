  PosibErr<String> Config::retrieve(ParmStr key) const
  {
    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);
    if (ki->type == KeyInfoList) return make_err(key_not_string, ki->name);

    const Entry * cur = lookup(ki->name);

    return cur ? cur->value : get_default(ki);
  }