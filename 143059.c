    **/
    bool is_sameXYC(const unsigned int size_x, const unsigned int size_y, const unsigned int size_c) const {
      return is_sameXY(size_x,size_y) && _spectrum==size_c;