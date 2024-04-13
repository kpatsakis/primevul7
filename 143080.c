    //! Return a 3x3 symmetric matrix containing specified coefficients.
    static CImg<T> tensor(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
      return matrix(a0,a1,a2,a1,a3,a4,a2,a4,a5);