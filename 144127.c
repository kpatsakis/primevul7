    **/
    T& atXYZ(const int x, const int y, const int z, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZ(): Empty instance.",
                                    cimg_instance);
      return _atXYZ(x,y,z,c);