    //! Load image from a file \newinstance.
    static CImg<T> get_load(const char *const filename) {
      return CImg<T>().load(filename);