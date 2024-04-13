     **/
    CImg<T>& tensor() {
      return get_tensor().move_to(*this);