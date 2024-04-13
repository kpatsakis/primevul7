    //! Replace the image by an identity matrix \newinstance.
    CImg<T> get_identity_matrix() const {
      return identity_matrix(std::max(_width,_height));