    //! Return specified range of image columns \inplace.
    CImg<T> get_columns(const int x0, const int x1) const {
      return get_crop(x0,0,0,0,x1,height() - 1,depth() - 1,spectrum() - 1);