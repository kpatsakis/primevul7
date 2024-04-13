    //! Resize image to dimensions of a display window \newinstance.
    CImg<T> get_resize(const CImgDisplay& disp,
                       const int interpolation_type=1, const unsigned int boundary_conditions=0,
                       const float centering_x = 0, const float centering_y = 0,
                       const float centering_z = 0, const float centering_c = 0) const {
      return get_resize(disp.width(),disp.height(),_depth,_spectrum,interpolation_type,boundary_conditions,
                        centering_x,centering_y,centering_z,centering_c);