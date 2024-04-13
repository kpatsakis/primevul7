    template<typename t>
    CImg<T>& draw_image(const CImg<t>& sprite, const float opacity=1) {
      return draw_image(0,sprite,opacity);