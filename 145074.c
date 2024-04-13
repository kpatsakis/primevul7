  void init(ParmString str, DataPair & d, String & buf)
  {
    const char * s = str;
    while (*s == ' ' || *s == '\t') ++s;
    size_t l = str.size() - (s - str);
    buf.assign(s, l);
    d.value.str  = buf.mstr();
    d.value.size = l;
  }