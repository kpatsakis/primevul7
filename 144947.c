    //! Compute the histogram of pixel values \newinstance.
    CImg<ulongT> get_histogram(const unsigned int nb_levels) const {
      if (!nb_levels || is_empty()) return CImg<ulongT>();
      T vmax = 0, vmin = min_max(vmax);
      return get_histogram(nb_levels,vmin,vmax);