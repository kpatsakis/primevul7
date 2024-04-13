    **/
    CImg<T> get_column(const int x0) const {
      return get_columns(x0,x0);