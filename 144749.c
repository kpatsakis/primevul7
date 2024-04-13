  template<typename T>
  inline CImg<_cimg_Tfloat> operator*(const char *const expression, const CImg<T>& img) {
    return img*expression;