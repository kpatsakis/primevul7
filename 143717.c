    **/
    bool contains(const T& pixel) const {
      const T *const ppixel = &pixel;
      return !is_empty() && ppixel>=_data && ppixel<_data + size();