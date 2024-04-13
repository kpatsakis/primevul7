    **/
    Tfloat cubic_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atX(): Empty instance.",
                                    cimg_instance);
      return _cubic_atX(fx,y,z,c);