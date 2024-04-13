     **/
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const double value0, const double value1, ...):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,(size_t)size_x*size_y*size_z*size_c,double);