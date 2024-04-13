    **/
    inline bool is_directory(const char *const path) {
      if (!path || !*path) return false;
#if cimg_OS==1
      struct stat st_buf;
      return (!stat(path,&st_buf) && S_ISDIR(st_buf.st_mode));
#elif cimg_OS==2
      const unsigned int res = (unsigned int)GetFileAttributesA(path);
      return res==INVALID_FILE_ATTRIBUTES?false:(res&16);
#else
      return false;
#endif