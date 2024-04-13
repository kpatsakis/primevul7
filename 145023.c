  void combine_list(String & res, const StringList & in)
  {
    res.clear();
    StringListEnumeration els = in.elements_obj();
    const char * s = 0;
    while ( (s = els.next()) != 0) 
    {
      for (; *s; ++s) {
        if (*s == ':')
          res.append('\\');
        res.append(*s);
      }
      res.append(':');
    }
    if (!res.empty() && res.back() == ':') res.pop_back();
  }