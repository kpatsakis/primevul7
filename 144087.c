    **/
    CImg<T> get_slice(const int z0) const {
      return get_slices(z0,z0);