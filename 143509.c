    **/
    CImg<T>& operator()(const unsigned int pos) {
#if cimg_verbosity>=3
      if (pos>=_width) {
        cimg::warn(_cimglist_instance
                   "operator(): Invalid image request, at position [%u].",
                   cimglist_instance,
                   pos);
        return *_data;
      }
#endif
      return _data[pos];