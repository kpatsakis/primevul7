    template<typename tp, typename tc, typename to>
    unsigned int _size_object3dtoCImg3d(const CImgList<tp>& primitives,
                                        const CImgList<tc>& colors,
                                        const CImgList<to>& opacities) const {
      unsigned int siz = 8U + 3*_width;
      cimglist_for(primitives,p) siz+=primitives[p].size() + 1;
      for (int c = std::min(primitives.width(),colors.width()) - 1; c>=0; --c) {
        if (colors[c].is_shared()) siz+=4;
        else { const unsigned int csiz = colors[c].size(); siz+=(csiz!=3)?4 + csiz:3; }
      }
      if (colors._width<primitives._width) siz+=3*(primitives._width - colors._width);
      cimglist_for(opacities,o) {
        if (opacities[o].is_shared()) siz+=4;
        else { const unsigned int osiz = opacities[o].size(); siz+=(osiz!=1)?4 + osiz:1; }
      }
      siz+=primitives._width - opacities._width;
      return siz;