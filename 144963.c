    //! Save image as a PFM file \overloading.
    const CImg<T>& save_pfm(std::FILE *const file) const {
      get_mirror('y')._save_pfm(file,0);
      return *this;