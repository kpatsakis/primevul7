    //! Load image from a DLM file \overloading.
    CImg<T>& load_dlm(std::FILE *const file) {
      return _load_dlm(file,0);