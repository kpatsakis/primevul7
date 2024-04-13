    **/
    CImg<T>& diagonal() {
      return get_diagonal().move_to(*this);