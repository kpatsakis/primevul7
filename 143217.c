    **/
    bool containsXYZC(const int x, const int y=0, const int z=0, const int c=0) const {
      return !is_empty() && x>=0 && x<width() && y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum();