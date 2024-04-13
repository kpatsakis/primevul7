    //! Compute the bitwise left rotation of each pixel value \newinstance.
    CImg<T> get_rol(const unsigned int n=1) const {
      return (+*this).rol(n);