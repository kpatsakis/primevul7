    //! Access to a pixel value, using Neumann boundary conditions for the X and Y-coordinates \const.
    const T& atXY(const int x, const int y, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXY(): Empty instance.",
                                    cimg_instance);
      return _atXY(x,y,z,c);