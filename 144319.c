    template<typename t>
    CImg(const std::initializer_list<t> values):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(values.size(),1,1,1);
      auto it = values.begin();
      unsigned int siz = _width;
      for (T *ptrd = _data; siz--; ) *(ptrd++) = (T)(*(it++));