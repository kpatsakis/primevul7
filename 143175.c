    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c,
                    const double value0, const double value1, ...) {
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,(size_t)size_x*size_y*size_z*size_c,double);
      return *this;