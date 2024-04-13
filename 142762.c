    template<typename t>
    CImg<T>& draw_image(const int x0, const int y0, const int z0,
                        const CImg<t>& sprite, const float opacity=1) {
      return draw_image(x0,y0,z0,0,sprite,opacity);