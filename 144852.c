    //! Compute distance to a specified value \newinstance.
    CImg<Tfloat> get_distance(const T& value, const unsigned int metric=2) const {
      return CImg<Tfloat>(*this,false).distance((Tfloat)value,metric);