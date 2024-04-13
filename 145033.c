  char * get_nb_line(IStream & in, String & buf)
  {
    char * p;
    // get first non blank line
    do {
      if (!in.getline(buf)) return 0;
      p = buf.mstr();
      while (*p == ' ' || *p == '\t') ++p;
    } while (*p == '#' || *p == '\0');
    return p;
  }