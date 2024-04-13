    //! Return a shared-memory image referencing one row of the image instance \const.
    const CImg<T> get_shared_row(const unsigned int y0, const unsigned int z0=0, const unsigned int c0=0) const {
      return get_shared_rows(y0,y0,z0,c0);