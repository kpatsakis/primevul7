    //! Return a 5x5 matrix containing specified coefficients.
    static CImg<T> matrix(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4,
                          const T& a5, const T& a6, const T& a7, const T& a8, const T& a9,
                          const T& a10, const T& a11, const T& a12, const T& a13, const T& a14,
                          const T& a15, const T& a16, const T& a17, const T& a18, const T& a19,
                          const T& a20, const T& a21, const T& a22, const T& a23, const T& a24) {
      CImg<T> r(5,5); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3; *(ptr++) = a4;
      *(ptr++) = a5; *(ptr++) = a6; *(ptr++) = a7; *(ptr++) = a8; *(ptr++) = a9;
      *(ptr++) = a10; *(ptr++) = a11; *(ptr++) = a12; *(ptr++) = a13; *(ptr++) = a14;
      *(ptr++) = a15; *(ptr++) = a16; *(ptr++) = a17; *(ptr++) = a18; *(ptr++) = a19;
      *(ptr++) = a20; *(ptr++) = a21; *(ptr++) = a22; *(ptr++) = a23; *(ptr++) = a24;
      return r;