    **/
    static CImg<T> vector(const T& a0, const T& a1, const T& a2) {
      CImg<T> r(1,3); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2;
      return r;