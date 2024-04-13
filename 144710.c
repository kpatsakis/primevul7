    template<typename tc>
    CImg<T>& draw_grid(const float delta_x,  const float delta_y,
                       const float offsetx, const float offsety,
                       const bool invertx, const bool inverty,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U) {
      if (is_empty()) return *this;
      CImg<uintT> seqx, seqy;
      if (delta_x!=0) {
        const float dx = delta_x>0?delta_x:_width*-delta_x/100;
        const unsigned int nx = (unsigned int)(_width/dx);
        seqx = CImg<uintT>::sequence(1 + nx,0,(unsigned int)(dx*nx));
        if (offsetx) cimg_foroff(seqx,x) seqx(x) = (unsigned int)cimg::mod(seqx(x) + offsetx,(float)_width);
        if (invertx) cimg_foroff(seqx,x) seqx(x) = _width - 1 - seqx(x);
      }
      if (delta_y!=0) {
        const float dy = delta_y>0?delta_y:_height*-delta_y/100;
        const unsigned int ny = (unsigned int)(_height/dy);
        seqy = CImg<uintT>::sequence(1 + ny,0,(unsigned int)(dy*ny));
        if (offsety) cimg_foroff(seqy,y) seqy(y) = (unsigned int)cimg::mod(seqy(y) + offsety,(float)_height);
        if (inverty) cimg_foroff(seqy,y) seqy(y) = _height - 1 - seqy(y);
     }
      return draw_grid(seqx,seqy,color,opacity,pattern_x,pattern_y);