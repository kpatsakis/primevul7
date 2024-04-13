    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int y0, const int z0,
                  const int x1, const int y1, const int z1,
                  const unsigned int boundary_conditions=0) {
      return crop(x0,y0,z0,0,x1,y1,z1,_spectrum - 1,boundary_conditions);