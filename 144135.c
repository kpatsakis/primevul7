    **/
    CImg<T>& columns(const int x0, const int x1) {
      return get_columns(x0,x1).move_to(*this);