  void separate_list(ParmStr value, AddableContainer & out, bool do_unescape)
  {
    unsigned len = value.size();
    
    VARARRAY(char, buf, len + 1);
    memcpy(buf, value, len + 1);
    
    len = strlen(buf);
    char * s = buf;
    char * end = buf + len;
      
    while (s < end)
    {
      if (do_unescape) while (*s == ' ' || *s == '\t') ++s;
      char * b = s;
      char * e = s;
      while (*s != '\0') {
        if (do_unescape && *s == '\\') {
          ++s;
          if (*s == '\0') break;
          e = s;
          ++s;
        } else {
          if (*s == ':') break;
          if (!do_unescape || (*s != ' ' && *s != '\t')) e = s;
          ++s;
        }
      }
      if (s != b) {
        ++e;
        *e = '\0';
        if (do_unescape) unescape(b);
      
        out.add(b);
      }
      ++s;
    }
  }