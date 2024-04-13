    **/
    Tfloat cubic_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXY(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXY(fx,fy,z,c);