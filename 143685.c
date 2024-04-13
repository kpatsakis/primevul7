    //! Compute the histogram of pixel values \overloading.
    CImg<T>& histogram(const unsigned int nb_levels) {
      return get_histogram(nb_levels).move_to(*this);