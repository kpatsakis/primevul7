    **/
    CImg<T> operator<<(const char *const expression) const {
      return (+*this)<<=expression;