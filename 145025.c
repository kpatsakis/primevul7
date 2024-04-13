  void truncate_file(FStream & f, ParmString name) {
#ifdef USE_FILE_LOCKS
    f.restart();
    ftruncate(f.file_no(),0);
#else
    f.close();
    f.open(name, "w+");
#endif
  }