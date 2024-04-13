    **/
    inline void warn(const char *const format, ...) {
      if (cimg::exception_mode()>=1) {
        char *const message = new char[16384];
        std::va_list ap;
        va_start(ap,format);
        cimg_vsnprintf(message,16384,format,ap);
        va_end(ap);
#ifdef cimg_strict_warnings
        throw CImgWarningException(message);
#else
        std::fprintf(cimg::output(),"\n%s[CImg] *** Warning ***%s%s\n",cimg::t_red,cimg::t_normal,message);
#endif
        delete[] message;
      }