    //! Blur image using Van Vliet recursive Gaussian filter. \newinstance.
    CImg<Tfloat> get_vanvliet(const float sigma, const unsigned int order, const char axis='x',
                              const bool boundary_conditions=true) const {
      return CImg<Tfloat>(*this,false).vanvliet(sigma,order,axis,boundary_conditions);