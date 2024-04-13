    **/
    static CImg<T> vector(const T& a0) {
      CImg<T> r(1,1);
      r[0] = a0;
      return r;