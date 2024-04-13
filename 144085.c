    //! Linearly normalize pixel values \newinstance.
    CImg<Tfloat> get_normalize(const T& min_value, const T& max_value) const {
      return CImg<Tfloat>(*this,false).normalize((Tfloat)min_value,(Tfloat)max_value);