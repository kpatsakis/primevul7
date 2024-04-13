    **/
    CImg<T>& round(const double y=1, const int rounding_type=0) {
      if (y>0) cimg_openmp_for(*this,cimg::round(*ptr,y,rounding_type),8192);
      return *this;