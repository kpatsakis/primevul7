    //! Load image from a DLM file \newinstance.
    static CImg<T> get_load_dlm(const char *const filename) {
      return CImg<T>().load_dlm(filename);