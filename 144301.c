    **/
    CImg<T> operator>(const char axis) const {
      return get_append(axis,0);