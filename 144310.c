    **/
    static CImg<T> matrix(const T& a0, const T& a1,
			  const T& a2, const T& a3) {
      CImg<T> r(2,2); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1;
      *(ptr++) = a2; *(ptr++) = a3;
      return r;