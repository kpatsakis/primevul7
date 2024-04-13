    template<typename t>
    bool contains(const T& pixel, t& x, t& y, t& z, t& c) const {
      const ulongT wh = (ulongT)_width*_height, whd = wh*_depth, siz = whd*_spectrum;
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
      ulongT off = (ulongT)(ppixel - _data);
      const ulongT nc = off/whd;
      off%=whd;
      const ulongT nz = off/wh;
      off%=wh;
      const ulongT ny = off/_width, nx = off%_width;
      x = (t)nx; y = (t)ny; z = (t)nz; c = (t)nc;
      return true;