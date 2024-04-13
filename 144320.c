    **/
    Tfloat cubic_atXYZ_p(const float fx, const float fy, const float fz, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXYZ_p(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXYZ_p(fx,fy,fz,c);