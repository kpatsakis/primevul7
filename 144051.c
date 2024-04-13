    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int y0,
                  const int x1, const int y1,
                  const unsigned int boundary_conditions=0) {
      return crop(x0,y0,0,0,x1,y1,_depth - 1,_spectrum - 1,boundary_conditions);