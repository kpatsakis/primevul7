    //! Access to a pixel value, using Dirichlet boundary conditions for the X and Y coordinates \const.
    T atXY(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || y<0 || x>=width() || y>=height())?out_value:(*this)(x,y,z,c);