  String add_possible_dir(ParmString dir, ParmString file) {
    if (need_dir(file)) {
      String path;
      path += dir;
      path += '/';
      path += file;
      return path;
    } else {
      return file;
    }
  }