    **/
    CImg<T> get_shared_slice(const unsigned int z0, const unsigned int c0=0) {
      return get_shared_slices(z0,z0,c0);