    **/
    Tfloat linear_atXYZC(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZC(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZC(fx,fy,fz,fc);