    //! Load image from a EXR file \newinstance.
    static CImg<T> get_load_exr(const char *const filename) {
      return CImg<T>().load_exr(filename);