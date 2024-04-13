    //! Resize image to become a scalar square matrix \newinstance.
    CImg<T> get_matrix() const {
      return (+*this).matrix();