    //! Blur image isotropically \newinstance.
    CImg<Tfloat> get_blur(const float sigma, const bool boundary_conditions=true, const bool is_gaussian=false) const {
      return CImg<Tfloat>(*this,false).blur(sigma,boundary_conditions,is_gaussian);