    **/
    CImg<T> get_channels(const int c0, const int c1) const {
      return get_crop(0,0,0,c0,width() - 1,height() - 1,depth() - 1,c1);