    template<typename t, typename tc>
    CImg<T>& draw_polygon(const CImg<t>& points,
                          const tc *const color, const float opacity, const unsigned int pattern) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points._width==1) return draw_point((int)points(0,0),(int)points(0,1),color,opacity);
      if (points._width==2) return draw_line((int)points(0,0),(int)points(0,1),
                                             (int)points(1,0),(int)points(1,1),color,opacity,pattern);
      bool ninit_hatch = true;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      default : {
        CImg<intT> npoints(points._width,2);
        int x = npoints(0,0) = (int)points(0,0), y = npoints(0,1) = (int)points(0,1);
        unsigned int nb_points = 1;
        for (unsigned int p = 1; p<points._width; ++p) {
          const int nx = (int)points(p,0), ny = (int)points(p,1);
          if (nx!=x || ny!=y) { npoints(nb_points,0) = nx; npoints(nb_points++,1) = ny; x = nx; y = ny; }
        }
        const int x0 = (int)npoints(0,0), y0 = (int)npoints(0,1);
        int ox = x0, oy = y0;
        for (unsigned int i = 1; i<nb_points; ++i) {
          const int _x = (int)npoints(i,0), _y = (int)npoints(i,1);
          draw_line(ox,oy,_x,_y,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = _x; oy = _y;
        }
        draw_line(ox,oy,x0,y0,color,opacity,pattern,false);
      }
      }
      return *this;