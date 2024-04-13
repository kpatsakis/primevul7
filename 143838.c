
    CImgDisplay& assign(const unsigned int dimw, const unsigned int dimh, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool fullscreen_flag=false, const bool closed_flag=false) {
      if (!dimw || !dimh) return assign();
      _assign(dimw,dimh,title,normalization_type,fullscreen_flag,closed_flag);
      _min = _max = 0;
      std::memset(_data,0,sizeof(unsigned int)*_width*_height);
      return paint();