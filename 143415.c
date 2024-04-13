    //! Shift image content \newinstance.
    CImg<T> get_shift(const int delta_x, const int delta_y=0, const int delta_z=0, const int delta_c=0,
                      const unsigned int boundary_conditions=0) const {
      return (+*this).shift(delta_x,delta_y,delta_z,delta_c,boundary_conditions);