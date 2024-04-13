    //! Normalize multi-valued pixels of the image instance, with respect to their L2-norm \newinstance.
    CImg<Tfloat> get_normalize() const {
      return CImg<Tfloat>(*this,false).normalize();