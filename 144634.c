    template<typename tp, typename tc, typename to>
    unsigned int _size_object3dtoCImg3d(const CImgList<tp>& primitives,
                                        const CImgList<tc>& colors,
                                        const CImg<to>& opacities) const {
      unsigned int siz = 8U + 3*_width;
      cimglist_for(primitives,p) siz+=primitives[p].size() + 1;
      for (int c = std::min(primitives.width(),colors.width()) - 1; c>=0; --c) {
        const unsigned int csiz = colors[c].size(); siz+=(csiz!=3)?4 + csiz:3;
      }
      if (colors._width<primitives._width) siz+=3*(primitives._width - colors._width);
      siz+=primitives.size();
      cimg::unused(opacities);
      return siz;