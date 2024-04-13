    //! Return pixel value, using linear interpolation and periodic boundary conditions for the X,Y and Z-coordinates.
    Tfloat linear_atXYZ_p(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZ_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZ_p(fx,fy,fz,c);