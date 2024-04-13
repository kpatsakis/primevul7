    //! Load image from a MINC2 file \newinstance.
    static CImg<T> get_load_minc2(const char *const filename) {
      return CImg<T>().load_analyze(filename);