    //! Load image using various non-native ways \newinstance.
    static CImg<T> get_load_other(const char *const filename) {
      return CImg<T>().load_other(filename);