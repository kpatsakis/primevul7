    //! Save image as a PNK file \overloading.
    const CImg<T>& save_pnk(std::FILE *const file) const {
      return _save_pnk(file,0);