    //! Return specified range of image slices \inplace.
    CImg<T>& slices(const int z0, const int z1) {
      return get_slices(z0,z1).move_to(*this);