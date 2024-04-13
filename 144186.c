    template<typename tc1, typename tc2, typename tc3>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const tc1 *const color1,
                           const tc2 *const color2,
                           const tc3 *const color3,
                           const float opacity=1) {
      const unsigned char one = 1;
      cimg_forC(*this,c)
        get_shared_channel(c).draw_triangle(x0,y0,x1,y1,x2,y2,&one,color1[c],color2[c],color3[c],opacity);
      return *this;