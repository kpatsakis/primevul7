    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0, const int y0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(x0,y0,0,sprite,mask,opacity,mask_max_value);