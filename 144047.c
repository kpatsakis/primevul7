    //! Equalize histogram of pixel values \newinstance.
    CImg<T> get_equalize(const unsigned int nblevels, const T& val_min, const T& val_max) const {
      return (+*this).equalize(nblevels,val_min,val_max);