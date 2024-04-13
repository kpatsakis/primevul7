    template<typename t>
    CImg(const unsigned int size_x, const unsigned int size_y,
         std::initializer_list<t> values,
	 const bool repeat_values=true):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x,size_y);
      _cimg_constructor_cpp11(repeat_values);