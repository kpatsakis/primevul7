  PosibErr<bool> open_file_writelock(FStream & inout, ParmString file) {
    typedef PosibErr<bool> Ret;
#ifndef USE_FILE_LOCKS
    bool exists = file_exists(file);
#endif
    {
     Ret pe = inout.open(file, "r+");
     if (pe.get_err() != 0)
       pe = inout.open(file, "w+");
     if (pe.has_err())
       return pe;
    }
#ifdef USE_FILE_LOCKS
    int fd = inout.file_no();
    struct flock fl;
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fcntl(fd, F_SETLKW, &fl); // ignore errors
    struct stat s;
    fstat(fd, &s);
    return s.st_size != 0;
#else
    return exists;
#endif
  }