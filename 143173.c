    //! Apply recursive Deriche filter \newinstance.
    CImg<Tfloat> get_deriche(const float sigma, const unsigned int order=0, const char axis='x',
                             const bool boundary_conditions=true) const {
      return CImg<Tfloat>(*this,false).deriche(sigma,order,axis,boundary_conditions);