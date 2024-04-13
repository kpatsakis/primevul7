    template<typename t>
    CImg<T> get_resize(const CImg<t>& src,
                       const int interpolation_type=1, const unsigned int boundary_conditions=0,
                       const float centering_x = 0, const float centering_y = 0,
                       const float centering_z = 0, const float centering_c = 0) const {
      return get_resize(src._width,src._height,src._depth,src._spectrum,interpolation_type,boundary_conditions,
                        centering_x,centering_y,centering_z,centering_c);