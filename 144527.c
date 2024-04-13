     **/
    static CImg<T> sequence(const unsigned int N, const T& a0, const T& a1) {
      if (N) return CImg<T>(1,N).sequence(a0,a1);
      return CImg<T>();