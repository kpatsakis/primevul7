  const Config::Entry * Config::lookup(const char * key) const
  {
    const Entry * res = 0;
    const Entry * cur = first_;

    while (cur) {
      if (cur->key == key && cur->action != NoOp)  res = cur;
      cur = cur->next;
    }

    if (!res || res->action == Reset) return 0;
    return res;
  }