    //! Generate a numbered version of a filename.
    inline char* number_filename(const char *const filename, const int number,
                                 const unsigned int digits, char *const str) {
      if (!filename) { if (str) *str = 0; return 0; }
      char *const format = new char[1024], *const body = new char[1024];
      const char *const ext = cimg::split_filename(filename,body);
      if (*ext) cimg_snprintf(format,1024,"%%s_%%.%ud.%%s",digits);
      else cimg_snprintf(format,1024,"%%s_%%.%ud",digits);
      cimg_sprintf(str,format,body,number,ext);
      delete[] format; delete[] body;
      return str;