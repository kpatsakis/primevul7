    //! Save image as a RGB file \overloading.
    const CImg<T>& save_rgb(std::FILE *const file) const {
      return _save_rgb(file,0);