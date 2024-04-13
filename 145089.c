  bool need_dir(ParmString file) {
    if (file[0] == '/' || (file[0] == '.' && file[1] == '/')
#ifdef WIN32
        || (asc_isalpha(file[0]) && file[1] == ':')
        || file[0] == '\\' || (file[0] == '.' && file[1] == '\\')
#endif
      )
      return false;
    else
      return true;
  }