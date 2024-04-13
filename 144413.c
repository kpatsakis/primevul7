    //! Fill pixel values along the Y-axis at a specified pixel position \overloading.
    CImg<T>& fillY(const unsigned int x, const unsigned int z, const unsigned int c, const double a0, ...) {
      if (x<_width && z<_depth && c<_spectrum) _cimg_fill1(x,0,z,c,_width,_height,double);
      return *this;