    **/
    CImg<T>& label(const bool is_high_connectivity=false, const Tfloat tolerance=0) {
      return get_label(is_high_connectivity,tolerance).move_to(*this);