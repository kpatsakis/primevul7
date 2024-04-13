    **/
    explicit CImgList(const unsigned int n):_width(n) {
      if (n) _data = new CImg<T>[_allocated_width = std::max(16U,(unsigned int)cimg::nearest_pow2(n))];
      else { _allocated_width = 0; _data = 0; }