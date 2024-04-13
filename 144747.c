    template<typename t>
    CImg<T> & operator=(std::initializer_list<t> values) {
      _cimg_constructor_cpp11(siz>values.size());
      return *this;