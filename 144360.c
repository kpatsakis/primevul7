    **/
    CImgList<T>& assign(const unsigned int n) {
      if (!n) return assign();
      if (_allocated_width<n || _allocated_width>(n<<2)) {
        delete[] _data;
        _data = new CImg<T>[_allocated_width = std::max(16U,(unsigned int)cimg::nearest_pow2(n))];
      }
      _width = n;
      return *this;