    template<typename T>
    inline int fdate(const char *const path, T *attr, const unsigned int nb_attr) {
#define _cimg_fdate_err() for (unsigned int i = 0; i<nb_attr; ++i) attr[i] = (T)-1
      int res = -1;
      if (!path || !*path) { _cimg_fdate_err(); return -1; }
      cimg::mutex(6);
#if cimg_OS==2
      HANDLE file = CreateFileA(path,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if (file!=INVALID_HANDLE_VALUE) {
        FILETIME _ft;
        SYSTEMTIME ft;
        if (GetFileTime(file,0,0,&_ft) && FileTimeToSystemTime(&_ft,&ft)) {
          for (unsigned int i = 0; i<nb_attr; ++i) {
            res = (int)(attr[i]==0?ft.wYear:attr[i]==1?ft.wMonth:attr[i]==2?ft.wDay:
                        attr[i]==3?ft.wDayOfWeek:attr[i]==4?ft.wHour:attr[i]==5?ft.wMinute:
                        attr[i]==6?ft.wSecond:-1);
            attr[i] = (T)res;
          }
        } else _cimg_fdate_err();
        CloseHandle(file);
      } else _cimg_fdate_err();
#elif cimg_OS==1
      struct stat st_buf;
      if (!stat(path,&st_buf)) {
        const time_t _ft = st_buf.st_mtime;
        const struct tm& ft = *std::localtime(&_ft);
        for (unsigned int i = 0; i<nb_attr; ++i) {
          res = (int)(attr[i]==0?ft.tm_year + 1900:attr[i]==1?ft.tm_mon + 1:attr[i]==2?ft.tm_mday:
                      attr[i]==3?ft.tm_wday:attr[i]==4?ft.tm_hour:attr[i]==5?ft.tm_min:
                      attr[i]==6?ft.tm_sec:-1);
          attr[i] = (T)res;
        }
      } else _cimg_fdate_err();
#endif
      cimg::mutex(6,0);
      return res;