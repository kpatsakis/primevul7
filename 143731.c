    **/
    CImg<T>& pseudoinvert() {
      return get_pseudoinvert().move_to(*this);