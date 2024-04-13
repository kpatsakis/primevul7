  unsigned find_file(const StringList & sl, String & filename)
  {
    StringListEnumeration els = sl.elements_obj();
    const char * dir;
    String path;
    while ( (dir = els.next()) != 0 ) 
    {
      path = dir;
      if (path.empty()) continue;
      if (path.back() != '/') path += '/';
      unsigned dir_len = path.size();
      path += filename;
      if (file_exists(path)) {
        filename.swap(path);
        return dir_len;
      }
    }
    return 0;
  }