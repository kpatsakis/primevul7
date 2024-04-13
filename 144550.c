    //! Save image as a RGBA file \overloading.
    const CImg<T>& save_rgba(std::FILE *const file) const {
      return _save_rgba(file,0);