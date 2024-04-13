    **/
    CImg<T>& norm(const int norm_type=2) {
      if (_spectrum==1 && norm_type) return abs();
      return get_norm(norm_type).move_to(*this);