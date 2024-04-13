    //! Return specified image row.
    CImg<T> get_row(const int y0) const {
      return get_rows(y0,y0);