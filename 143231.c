     **/
    T& atXY(const int x, const int y, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXY(): Empty instance.",
                                    cimg_instance);
      return _atXY(x,y,z,c);