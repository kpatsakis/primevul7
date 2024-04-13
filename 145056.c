  bool getdata_pair(IStream & in, DataPair & d, String & buf)
  {
    char * p;

    // get first non blank line and count all read ones
    do {
      buf.clear();
      buf.append('\0'); // to avoid some special cases
      if (!in.append_line(buf)) return false;
      d.line_num++;
      p = buf.mstr() + 1;
      while (*p == ' ' || *p == '\t') ++p;
    } while (*p == '#' || *p == '\0');

    // get key
    d.key.str = p;
    while (*p != '\0' &&
           ((*p != ' ' && *p != '\t' && *p != '#') || *(p-1) == '\\')) ++p;
    d.key.size = p - d.key.str;

    // figure out if there is a value and add terminate key
    d.value.str = p; // in case there is no value
    d.value.size = 0;
    if (*p == '#' || *p == '\0') {*p = '\0'; return true;}
    *p = '\0';

    // skip any whitespace
    ++p;
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0' || *p == '#') {return true;}

    // get value
    d.value.str = p;
    while (*p != '\0' && (*p != '#' || *(p-1) == '\\')) ++p;
    
    // remove trailing white space and terminate value
    --p;
    while (*p == ' ' || *p == '\t') --p;
    if (*p == '\\' && *(p + 1) != '\0') ++p;
    ++p;
    d.value.size = p - d.value.str;
    *p = '\0';

    return true;
  }