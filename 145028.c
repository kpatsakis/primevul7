  const char * Config::base_name(const char * s, Action * action)
  {
    if (action) *action = Set;
    const char * c = strchr(s, '-');
    if (!c) return s;
    unsigned len = c - s;
    TEST("reset",   5, Reset);
    TEST("enable",  6, Enable);
    TEST("dont",    4, Disable);
    TEST("disable", 7, Disable);
    TEST("lset",    4, ListSet);
    TEST("rem",     3, ListRemove);
    TEST("remove",  6, ListRemove);
    TEST("add",     3, ListAdd);
    TEST("clear",   5, ListClear);
    return s;
  }