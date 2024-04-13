    **/
    CImg<T>& identity_matrix() {
      return identity_matrix(std::max(_width,_height)).move_to(*this);