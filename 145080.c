  PosibErr<String> Config::retrieve_any(ParmStr key) const
  {
    RET_ON_ERR_SET(keyinfo(key), const KeyInfo *, ki);

    if (ki->type != KeyInfoList) {
      const Entry * cur = lookup(ki->name);
      return cur ? cur->value : get_default(ki);
    } else {
      StringList sl;
      RET_ON_ERR(retrieve_list(key, &sl));
      StringListEnumeration els = sl.elements_obj();
      const char * s;
      String val;
      while ( (s = els.next()) != 0 ) {
        val += s;
        val += '\n';
      }
      val.pop_back();
      return val;
    }
  }