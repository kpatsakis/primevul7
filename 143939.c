    template<typename tf>
    static CImg<floatT> plane3d(CImgList<tf>& primitives,
                                const float size_x=100, const float size_y=100,
                                const unsigned int subdivisions_x=10, const unsigned int subdivisions_y=10) {
      primitives.assign();
      if (!subdivisions_x || !subdivisions_y) return CImg<floatT>();
      CImgList<floatT> vertices;
      const unsigned int w = subdivisions_x + 1, h = subdivisions_y + 1;
      const float fx = (float)size_x/w, fy = (float)size_y/h;
      for (unsigned int y = 0; y<h; ++y) for (unsigned int x = 0; x<w; ++x)
        CImg<floatT>::vector(fx*x,fy*y,0).move_to(vertices);
      for (unsigned int y = 0; y<subdivisions_y; ++y) for (unsigned int x = 0; x<subdivisions_x; ++x) {
        const int off1 = x + y*w, off2 = x + 1 + y*w, off3 = x + 1 + (y + 1)*w, off4 = x + (y + 1)*w;
	CImg<tf>::vector(off1,off4,off3,off2).move_to(primitives);
      }
      return vertices>'x';