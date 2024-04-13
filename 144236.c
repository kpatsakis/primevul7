    //! Fill pixel values along the C-axis at a specified pixel position \overloading.
    CImg<T>& fillC(const unsigned int x, const unsigned int y, const unsigned int z, const double a0, ...) {
      const ulongT whd = (ulongT)_width*_height*_depth;
      if (x<_width && y<_height && z<_depth) _cimg_fill1(x,y,z,0,whd,_spectrum,double);
      return *this;