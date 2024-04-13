    //! Resize image to half-size along XY axes, using an optimized filter.
    CImg<T>& resize_halfXY() {
      return get_resize_halfXY().move_to(*this);