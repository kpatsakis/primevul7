    //! Fill pixel values along the X-axis at a specified pixel position \overloading.
    CImg<T>& fillX(const unsigned int y, const unsigned int z, const unsigned int c, const double a0, ...) {
      if (y<_height && z<_depth && c<_spectrum) _cimg_fill1(0,y,z,c,1,_width,double);
      return *this;