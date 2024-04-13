    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0,
                            const int x1, const int y1, const int z1,
                            const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_rectangle(): Specified color is (null).",
                                    cimg_instance);
      cimg_forC(*this,c) draw_rectangle(x0,y0,z0,c,x1,y1,z1,c,(T)color[c],opacity);
      return *this;