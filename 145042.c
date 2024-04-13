  bool file_exists(ParmString name) {
    return ACCESS(name, F_OK) == 0;
  }