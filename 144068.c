    **/
    Tfloat linear_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atX(): Empty instance.",
                                    cimg_instance);

      return _linear_atX(fx,y,z,c);