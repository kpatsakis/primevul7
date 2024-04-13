    //! Load image from a JPEG file \newinstance.
    static CImg<T> get_load_jpeg(const char *const filename) {
      return CImg<T>().load_jpeg(filename);