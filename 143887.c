    //! Construct image with specified size and initialize pixel values from a memory buffer \overloading.
    CImg<T>& assign(const T *const values, const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const bool is_shared) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (!values || !siz) return assign();
      if (!is_shared) { if (_is_shared) assign(); assign(values,size_x,size_y,size_z,size_c); }
      else {
	if (!_is_shared) {
	  if (values + siz<_data || values>=_data + size()) assign();
	  else cimg::warn(_cimg_instance
                          "assign(): Shared image instance has overlapping memory.",
                          cimg_instance);
	}
	_width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c; _is_shared = true;
	_data = const_cast<T*>(values);
      }
      return *this;