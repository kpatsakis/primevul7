
    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      if (is_empty()) return *this;
      if (force_redraw) {
        const cimg_ulong buf_size = (cimg_ulong)_width*_height*
          (cimg::X11_attr().nb_bits==8?1:(cimg::X11_attr().nb_bits==16?2:4));
        void *image_data = std::malloc(buf_size);
        std::memcpy(image_data,_data,buf_size);
        assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
        std::memcpy(_data,image_data,buf_size);
        std::free(image_data);
        return paint();
      }
      return assign(_width,_height,_title,_normalization,!_is_fullscreen,false);