    //! Save image as a PNM file \overloading.
    const CImg<T>& save_pnm(std::FILE *const file, const unsigned int bytes_per_pixel=0) const {
      return _save_pnm(file,0,bytes_per_pixel);