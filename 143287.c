    //! Load image from a DLM file \newinstance.
    static CImg<T> get_load_dlm(std::FILE *const file) {
      return CImg<T>().load_dlm(file);