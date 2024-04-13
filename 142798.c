    **/
    CImg<charT> value_string(const char separator=',', const unsigned int max_size=0,
                             const char *const format=0) const {
      if (is_empty() || max_size==1) return CImg<charT>(1,1,1,1,0);
      CImgList<charT> items;
      CImg<charT> s_item(256); *s_item = 0;
      const T *ptrs = _data;
      unsigned int string_size = 0;
      const char *const _format = format?format:cimg::type<T>::format();
      for (ulongT off = 0, siz = size(); off<siz && (!max_size || string_size<max_size); ++off) {
        const unsigned int printed_size = 1U + cimg_snprintf(s_item,s_item._width,_format,
                                                             cimg::type<T>::format(*(ptrs++)));
        CImg<charT> item(s_item._data,printed_size);
        item[printed_size - 1] = separator;
        item.move_to(items);
        if (max_size) string_size+=printed_size;
      }
      CImg<charT> res;
      (items>'x').move_to(res);
      if (max_size && res._width>=max_size) res.crop(0,max_size - 1);
      res.back() = 0;
      return res;