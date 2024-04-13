    template<typename t, typename tc>
    CImg<T>& draw_point(const CImg<t>& points,
                        const tc *const color, const float opacity=1) {
      if (is_empty() || !points) return *this;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_point(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);
      case 2 : {
        cimg_forX(points,i) draw_point((int)points(i,0),(int)points(i,1),color,opacity);
      } break;
      default : {
        cimg_forX(points,i) draw_point((int)points(i,0),(int)points(i,1),(int)points(i,2),color,opacity);
      }
      }
      return *this;