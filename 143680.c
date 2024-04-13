    **/
    T& atXYZC(const int x, const int y, const int z, const int c) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZC(): Empty instance.",
                                    cimg_instance);
      return _atXYZC(x,y,z,c);