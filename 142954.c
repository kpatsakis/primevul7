    template<typename t>
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const std::initializer_list<t> values,
	 const bool repeat_values=true):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
#define _cimg_constructor_cpp11(repeat_values) \
  auto it = values.begin(); \
  size_t siz = size(); \
  if (repeat_values) for (T *ptrd = _data; siz--; ) { \
    *(ptrd++) = (T)(*(it++)); if (it==values.end()) it = values.begin(); } \
  else { siz = std::min(siz,values.size()); for (T *ptrd = _data; siz--; ) *(ptrd++) = (T)(*(it++)); }
      assign(size_x,size_y,size_z,size_c);
      _cimg_constructor_cpp11(repeat_values);