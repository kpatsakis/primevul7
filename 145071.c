  time_t get_modification_time(FStream & f) {
    struct stat s;
    fstat(f.file_no(), &s);
    return s.st_mtime;
  }