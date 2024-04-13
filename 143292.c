    **/
    CImg<T>& load_gif_external(const char *const filename,
                               const char axis='z', const float align=0) {
      return get_load_gif_external(filename,axis,align).move_to(*this);