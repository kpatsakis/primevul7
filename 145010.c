  PosibErr<bool> Config::retrieve_bool(ParmStr key) const
  {
    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);
    if (ki->type != KeyInfoBool) return make_err(key_not_bool, ki->name);

    const Entry * cur = lookup(ki->name);

    String value(cur ? cur->value : get_default(ki));

    if (value == "false") return false;
    else                  return true;
  }