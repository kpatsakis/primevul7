#endif
    }

    // Open a file (similar to std:: fopen(), but with wide character support on Windows).
    inline std::FILE *std_fopen(const char *const path, const char *const mode) {
      std::FILE *const res = std::fopen(path,mode);
      if (res) return res;
#if cimg_OS==2
      // Try alternative method, with wide-character string.
      int err = MultiByteToWideChar(CP_UTF8,0,path,-1,0,0);
      if (err) {
        CImg<wchar_t> wpath(err);
        err = MultiByteToWideChar(CP_UTF8,0,path,-1,wpath,err);
        if (err) { // Convert 'mode' to a wide-character string
          err = MultiByteToWideChar(CP_UTF8,0,mode,-1,0,0);
          if (err) {
            CImg<wchar_t> wmode(err);
            if (MultiByteToWideChar(CP_UTF8,0,mode,-1,wmode,err))
              return _wfopen(wpath,wmode);
          }
        }