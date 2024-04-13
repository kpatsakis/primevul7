  bool split(DataPair & d)
  {
    char * p   = d.value;
    char * end = p + d.value.size;
    d.key.str = p;
    while (p != end) {
      ++p;
      if ((*p == ' ' || *p == '\t') && *(p-1) != '\\') break;
    }
    d.key.size = p - d.key.str;
    *p = 0;
    if (p != end) {
      ++p;
      while (p != end && (*p == ' ' || *p == '\t')) ++p;
    }
    d.value.str = p;
    d.value.size = end - p;
    return d.key.size != 0;
  }