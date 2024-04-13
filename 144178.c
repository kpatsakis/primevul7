    //! Compute the histogram of pixel values \newinstance.
    CImg<ulongT> get_histogram(const unsigned int nb_levels, const T& min_value, const T& max_value) const {
      if (!nb_levels || is_empty()) return CImg<ulongT>();
      const double
        vmin = (double)(min_value<max_value?min_value:max_value),
        vmax = (double)(min_value<max_value?max_value:min_value);
      CImg<ulongT> res(nb_levels,1,1,1,0);
      cimg_rof(*this,ptrs,T) {
        const T val = *ptrs;
        if (val>=vmin && val<=vmax) ++res[val==vmax?nb_levels - 1:(unsigned int)((val - vmin)*nb_levels/(vmax - vmin))];
      }
      return res;