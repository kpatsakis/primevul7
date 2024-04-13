    **/
    inline int fclose(std::FILE *file) {
      if (!file) { warn("cimg::fclose(): Specified file is (null)."); return 0; }
      if (file==cimg::_stdin(false) || file==cimg::_stdout(false)) return 0;
      const int errn = std::fclose(file);
      if (errn!=0) warn("cimg::fclose(): Error code %d returned during file closing.",
                        errn);
      return errn;