    **/
    inline cimg_ulong time() {
#if cimg_OS==1
      struct timeval st_time;
      gettimeofday(&st_time,0);
      return (cimg_ulong)(st_time.tv_usec/1000 + st_time.tv_sec*1000);
#elif cimg_OS==2
      SYSTEMTIME st_time;
      GetLocalTime(&st_time);
      return (cimg_ulong)(st_time.wMilliseconds + 1000*(st_time.wSecond + 60*(st_time.wMinute + 60*st_time.wHour)));
#else
      return 0;
#endif