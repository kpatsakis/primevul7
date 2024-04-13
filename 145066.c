  PathBrowser::PathBrowser(const StringList & sl, const char * suf)
    : dir_handle(0)
  {
    els = sl.elements();
    suffix = suf;
  }