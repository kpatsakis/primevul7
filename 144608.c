    //! Return specified image slice \inplace.
    CImg<T>& slice(const int z0) {
      return slices(z0,z0);