    //! Blur image with a box filter \newinstance.
    CImg<Tfloat> get_blur_box(const float boxsize, const bool boundary_conditions=true) const {
      return CImg<Tfloat>(*this,false).blur_box(boxsize,boundary_conditions);