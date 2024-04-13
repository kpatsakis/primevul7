
    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      if (is_empty()) return *this;
      if (force_redraw) {
        const cimg_ulong buf_size = (cimg_ulong)_width*_height*4;
        void *odata = std::malloc(buf_size);
        if (odata) {
          std::memcpy(odata,_data,buf_size);
          assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
          std::memcpy(_data,odata,buf_size);
          std::free(odata);
        }
        return paint();
      }
      return assign(_width,_height,_title,_normalization,!_is_fullscreen,false);