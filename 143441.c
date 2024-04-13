    //! Return a \c 1x5 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
      CImg<T> r(1,5); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3; *(ptr++) = a4;
      return r;