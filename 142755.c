    template<typename tc>
    CImg<T>& draw_mandelbrot(const CImg<tc>& colormap, const float opacity=1,
                             const double z0r=-2, const double z0i=-2, const double z1r=2, const double z1i=2,
                             const unsigned int iteration_max=255,
                             const bool is_normalized_iteration=false,
                             const bool is_julia_set=false,
                             const double param_r=0, const double param_i=0) {
      return draw_mandelbrot(0,0,_width - 1,_height - 1,colormap,opacity,
                             z0r,z0i,z1r,z1i,iteration_max,is_normalized_iteration,is_julia_set,param_r,param_i);