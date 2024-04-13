    //! Equalize histogram of pixel values \overloading.
    CImg<T>& equalize(const unsigned int nb_levels) {
      if (!nb_levels || is_empty()) return *this;
      T vmax = 0, vmin = min_max(vmax);
      return equalize(nb_levels,vmin,vmax);