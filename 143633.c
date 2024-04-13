  template<typename T>
  inline bool operator==(const char *const expression, const CImg<T>& img) {
    return img==expression;