    //! Add random noise to pixel values \newinstance.
    CImg<T> get_noise(const double sigma, const unsigned int noise_type=0) const {
      return (+*this).noise(sigma,noise_type);