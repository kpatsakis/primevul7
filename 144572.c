    **/
    CImg<T>& structure_tensors(const bool is_fwbw_scheme=false) {
      return get_structure_tensors(is_fwbw_scheme).move_to(*this);