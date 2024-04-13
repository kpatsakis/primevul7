    //! Access to a pixel value, using Neumann boundary conditions for the X-coordinate \const.
    const T& atX(const int x, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atX(): Empty instance.",
                                    cimg_instance);
      return _atX(x,y,z,c);