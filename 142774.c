    template<typename t>
    bool contains(const T& pixel, t& x, t& y) const {
      const ulongT wh = (ulongT)_width*_height, siz = wh*_depth*_spectrum;
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
      ulongT off = ((unsigned int)(ppixel - _data))%wh;
      const ulongT ny = off/_width, nx = off%_width;
      x = (t)nx; y = (t)ny;
      return true;