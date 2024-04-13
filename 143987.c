    //! Compute the bitwise right rotation of each pixel value \newinstance.
    CImg<T> get_ror(const char *const expression) const {
      return (+*this).ror(expression);