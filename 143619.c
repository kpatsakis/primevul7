    //! Access to a pixel value, using Dirichlet boundary conditions \const.
    T atXYZC(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || y<0 || z<0 || c<0 || x>=width() || y>=height() || z>=depth() || c>=spectrum())?out_value:
        (*this)(x,y,z,c);