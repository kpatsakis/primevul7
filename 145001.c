  void remove_comments(String & buf)
  {
    char * p = buf.mstr();
    char * b = p;
    while (*p && *p != '#') ++p;
    if (*p == '#') {--p; while (p >= b && asc_isspace(*p)) --p; ++p;}
    buf.resize(p - b);
  }