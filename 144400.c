    //! Return a \c 1x15 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
			  const T& a12, const T& a13, const T& a14) {
      CImg<T> r(1,15); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3;
      *(ptr++) = a4; *(ptr++) = a5; *(ptr++) = a6; *(ptr++) = a7;
      *(ptr++) = a8; *(ptr++) = a9; *(ptr++) = a10; *(ptr++) = a11;
      *(ptr++) = a12; *(ptr++) = a13; *(ptr++) = a14;
      return r;