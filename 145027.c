  const char * PathBrowser::next()
  {
    if (dir_handle == 0) goto get_next_dir;
  begin: {
      struct dirent * entry = readdir((DIR *)dir_handle);
      if (entry == 0) goto try_again;
      const char * name = entry->d_name;
      unsigned name_len = strlen(name);
      if (suffix.size() != 0 && 
          !(name_len > suffix.size() 
            && memcmp(name + name_len - suffix.size(), suffix.str(), suffix.size()) == 0))
        goto begin;
      path = dir;
      if (path.back() != '/') path += '/';
      path += name;
    }
    return path.str();
  try_again:
    if (dir_handle) closedir((DIR *)dir_handle);
    dir_handle = 0;
  get_next_dir:
    dir = els->next();
    if (!dir) return 0;
    dir_handle = opendir(dir);
    if (dir_handle == 0) goto try_again;
    goto begin;
  }