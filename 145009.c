  PosibErr<int> Config::retrieve_int(ParmStr key) const
  {
    assert(committed_); // otherwise the value may not be an integer
                        // as it has not been verified.

    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);
    if (ki->type != KeyInfoInt) return make_err(key_not_int, ki->name);

    const Entry * cur = lookup(ki->name);

    String value(cur ? cur->value : get_default(ki));

    return atoi(value.str());
  }