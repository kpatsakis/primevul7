    template<typename ti, typename tm>
    CImg<T>& draw_image(const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(0,sprite,mask,opacity,mask_max_value);