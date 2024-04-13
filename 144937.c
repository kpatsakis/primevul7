    **/
    inline void sleep(const unsigned int milliseconds) {
#if cimg_OS==1
      struct timespec tv;
      tv.tv_sec = milliseconds/1000;
      tv.tv_nsec = (milliseconds%1000)*1000000;
      nanosleep(&tv,0);
#elif cimg_OS==2
      Sleep(milliseconds);
#else
      cimg::unused(milliseconds);
#endif