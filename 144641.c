    //! Return pixel value, using linear interpolation and periodic boundary conditions for the X and Y-coordinates.
    Tfloat linear_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXY_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atXY_p(fx,fy,z,c);