    //! Equalize histogram of pixel values \newinstance.
    CImg<T> get_equalize(const unsigned int nblevels) const {
      return (+*this).equalize(nblevels);