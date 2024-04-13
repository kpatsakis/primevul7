    //! Compute the bitwise left rotation of each pixel value \newinstance.
    CImg<T> get_rol(const char *const expression) const {
      return (+*this).rol(expression);