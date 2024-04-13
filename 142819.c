    //! Save image as a DLM file \overloading.
    const CImg<T>& save_dlm(std::FILE *const file) const {
      return _save_dlm(file,0);