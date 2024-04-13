    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const T& value) {
      return assign(size_x,size_y,size_z,size_c).fill(value);