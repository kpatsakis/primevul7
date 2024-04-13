    //! Return a 2x2 symmetric matrix tensor containing specified coefficients.
    static CImg<T> tensor(const T& a0, const T& a1, const T& a2) {
      return matrix(a0,a1,a1,a2);