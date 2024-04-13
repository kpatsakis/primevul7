    //! Access to a pixel value, using Dirichlet boundary conditions for the X-coordinate \const.
    T atX(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || x>=width())?out_value:(*this)(x,y,z,c);