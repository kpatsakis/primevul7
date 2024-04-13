    //! Unroll pixel values along axis \c y \newinstance.
    CImg<T> get_vector() const {
      return get_unroll('y');