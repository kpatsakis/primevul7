    **/
    Tfloat linear_atXYZ(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZ(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZ(fx,fy,fz,c);