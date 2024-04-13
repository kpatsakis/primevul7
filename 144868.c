    **/
    bool is_sameXYZ(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z) const {
      return is_sameXY(size_x,size_y) && _depth==size_z;