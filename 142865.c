    //! Return a 4x4 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1, const T& a2, const T& a3) {
      return matrix(a0,0,0,0,0,a1,0,0,0,0,a2,0,0,0,0,a3);