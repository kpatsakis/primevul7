    **/
    CImg<T> get_rows(const int y0, const int y1) const {
      return get_crop(0,y0,0,0,width() - 1,y1,depth() - 1,spectrum() - 1);