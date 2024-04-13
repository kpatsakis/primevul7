    //! Load image from a JPEG file \newinstance.
    static CImg<T> get_load_jpeg(std::FILE *const file) {
      return CImg<T>().load_jpeg(file);