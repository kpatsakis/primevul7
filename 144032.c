    **/
    longT offset(const int x, const int y=0, const int z=0, const int c=0) const {
      return x + (longT)y*_width + (longT)z*_width*_height + (longT)c*_width*_height*_depth;