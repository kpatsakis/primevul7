  template<typename T>
  inline CImg<T> operator|(const char *const expression, const CImg<T>& img) {
    return img | expression;