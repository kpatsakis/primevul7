    //! Blur image \newinstance.
    CImg<Tfloat> get_blur(const float sigma_x, const float sigma_y, const float sigma_z,
                          const bool boundary_conditions=true, const bool is_gaussian=false) const {
      return CImg<Tfloat>(*this,false).blur(sigma_x,sigma_y,sigma_z,boundary_conditions,is_gaussian);