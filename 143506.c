    //! Return a \c 1x6 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
      CImg<T> r(1,6); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3; *(ptr++) = a4; *(ptr++) = a5;
      return r;