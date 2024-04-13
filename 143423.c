
    bool _draw_fill(const int x, const int y, const int z,
                    const CImg<T>& ref, const float tolerance2) const {
      const T *ptr1 = data(x,y,z), *ptr2 = ref._data;
      const unsigned long off = _width*_height*_depth;
      float diff = 0;
      cimg_forC(*this,c) { diff += cimg::sqr(*ptr1 - *(ptr2++)); ptr1+=off; }
      return diff<=tolerance2;