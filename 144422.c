    //! Return a 2x2 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1) {
      return matrix(a0,0,0,a1);