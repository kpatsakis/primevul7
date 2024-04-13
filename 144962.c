    **/
    CImg<T> get_slices(const int z0, const int z1) const {
      return get_crop(0,0,z0,0,width() - 1,height() - 1,z1,spectrum() - 1);