    **/
    static CImg<T> vector(const T& a0, const T& a1) {
      CImg<T> r(1,2); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1;
      return r;