    **/
    inline cimg_long wait(const unsigned int milliseconds) {
      cimg::mutex(3);
      static cimg_ulong timer = cimg::time();
      cimg::mutex(3,0);
      return cimg::wait(milliseconds,&timer);