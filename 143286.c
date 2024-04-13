
    CImgDisplay& assign(const unsigned int dimw, const unsigned int dimh, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool fullscreen_flag=false, const bool closed_flag=false) {
      if (!dimw || !dimh) return assign();
      _assign(dimw,dimh,title,normalization_type,fullscreen_flag,closed_flag);
      _min = _max = 0;
      std::memset(_data,0,(cimg::X11_attr().nb_bits==8?sizeof(unsigned char):
                           (cimg::X11_attr().nb_bits==16?sizeof(unsigned short):sizeof(unsigned int)))*
                  (size_t)_width*_height);
      return paint();