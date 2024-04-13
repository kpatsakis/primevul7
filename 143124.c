    //! Compute the bitwise right rotation of each pixel value \newinstance.
    CImg<T> get_ror(const unsigned int n=1) const {
      return (+*this).ror(n);