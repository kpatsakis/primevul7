    template<typename t1, typename t2>
    CImg<T>& draw_quiver(const CImg<t1>& flow,
                         const t2 *const color, const float opacity=1,
                         const unsigned int sampling=25, const float factor=-20,
                         const bool is_arrow=true, const unsigned int pattern=~0U) {
      return draw_quiver(flow,CImg<t2>(color,_spectrum,1,1,1,true),opacity,sampling,factor,is_arrow,pattern);