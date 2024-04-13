    template<typename t>
    CImg<T>& draw_image(const int x0,
                        const CImg<t>& sprite, const float opacity=1) {
      return draw_image(x0,0,sprite,opacity);