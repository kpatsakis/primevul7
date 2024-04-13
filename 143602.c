    //! Compute statistics vector from the pixel values \inplace.
    CImg<T>& stats(const unsigned int variance_method=1) {
      return get_stats(variance_method).move_to(*this);