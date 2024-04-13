    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int x1, const unsigned int boundary_conditions=0) {
      return crop(x0,0,0,0,x1,_height - 1,_depth - 1,_spectrum - 1,boundary_conditions);