    template<typename t>
    bool contains(const T& pixel, t& x) const {
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + size()) return false;
      x = (t)(((ulongT)(ppixel - _data))%_width);
      return true;