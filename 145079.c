  PosibErr<void> open_file_readlock(FStream & in, ParmString file) {
    RET_ON_ERR(in.open(file, "r"));
#ifdef USE_FILE_LOCKS
    int fd = in.file_no();
    struct flock fl;
    fl.l_type   = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fcntl(fd, F_SETLKW, &fl); // ignore errors
#endif
    return no_err;
  }