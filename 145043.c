  const char * get_file_name(const char * path) {
    const char * file_name;
    if (path != 0) {
      file_name = strrchr(path,'/');
      if (file_name == 0)
        file_name = path;
    } else {
      file_name = 0;
    }
    return file_name;
  }