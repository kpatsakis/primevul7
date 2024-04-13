    **/
   T& atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZ(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZ(pos,x,y,z,c);