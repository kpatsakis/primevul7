    //! Blur image with a box filter \newinstance.
    CImg<Tfloat> get_blur_box(const float boxsize_x, const float boxsize_y, const float boxsize_z,
                              const bool boundary_conditions=true) const {
      return CImg<Tfloat>(*this,false).blur_box(boxsize_x,boxsize_y,boxsize_z,boundary_conditions);