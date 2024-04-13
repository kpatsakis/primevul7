    template<typename t>
    CImg<T>& label(const CImg<t>& connectivity_mask, const Tfloat tolerance=0) {
      return get_label(connectivity_mask,tolerance).move_to(*this);