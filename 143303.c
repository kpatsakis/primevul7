    **/
    inline std::FILE* output(std::FILE *file) {
      cimg::mutex(1);
      static std::FILE *res = cimg::_stderr();
      if (file) res = file;
      cimg::mutex(1,0);
      return res;