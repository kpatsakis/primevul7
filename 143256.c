    **/
    CImg<T>& permute_axes(const char *const order) {
      return get_permute_axes(order).move_to(*this);