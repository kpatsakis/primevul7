    **/
    Tfloat cubic_atXYZ(const float fx, const float fy, const float fz, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXYZ(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXYZ(fx,fy,fz,c);