    //! Compute distance function to 0-valued isophotes, using the Eikonal PDE \newinstance.
    CImg<Tfloat> get_distance_eikonal(const unsigned int nb_iterations, const float band_size=0,
                                      const float time_step=0.5f) const {
      return CImg<Tfloat>(*this,false).distance_eikonal(nb_iterations,band_size,time_step);