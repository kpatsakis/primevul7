    // Apply box filter of order 0,1 or 2 \newinstance.
    CImg<Tfloat> get_boxfilter(const float boxsize, const int order, const char axis='x',
                               const bool boundary_conditions=true,
                               const unsigned int nb_iter=1) const {
      return CImg<Tfloat>(*this,false).boxfilter(boxsize,order,axis,boundary_conditions,nb_iter);