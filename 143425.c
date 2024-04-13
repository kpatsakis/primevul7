    template<typename tp, typename tc>
    CImg<T>& draw_polygon(const CImg<tp>& points,
                          const tc *const color, const float opacity=1) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      if (points._width==1) return draw_point(cimg::uiround(points(0,0)),cimg::uiround(points(0,1)),color,opacity);
      if (points._width==2) return draw_line(cimg::uiround(points(0,0)),cimg::uiround(points(0,1)),
                                             cimg::uiround(points(1,0)),cimg::uiround(points(1,1)),color,opacity);
      if (points._width==3) return draw_triangle(cimg::uiround(points(0,0)),cimg::uiround(points(0,1)),
                                                 cimg::uiround(points(1,0)),cimg::uiround(points(1,1)),
                                                 cimg::uiround(points(2,0)),cimg::uiround(points(2,1)),color,opacity);
      cimg_init_scanline(opacity);
      int
        xmin = 0, ymin = 0,
        xmax = points.get_shared_row(0).max_min(xmin),
        ymax = points.get_shared_row(1).max_min(ymin);
      if (xmax<0 || xmin>=width() || ymax<0 || ymin>=height()) return *this;
      if (ymin==ymax) return draw_line(xmin,ymin,xmax,ymax,color,opacity);

      ymin = std::max(0,ymin);
      ymax = std::min(height() - 1,ymax);
      CImg<intT> Xs(points._width,ymax - ymin + 1);
      CImg<uintT> count(Xs._height,1,1,1,0);
      unsigned int n = 0, nn = 1;
      bool go_on = true;

      while (go_on) {
        unsigned int an = (nn + 1)%points._width;
        const int
          x0 = cimg::uiround(points(n,0)),
          y0 = cimg::uiround(points(n,1));
        if (points(nn,1)==y0) while (points(an,1)==y0) { nn = an; (an+=1)%=points._width; }
        const int
          x1 = cimg::uiround(points(nn,0)),
          y1 = cimg::uiround(points(nn,1));
        unsigned int tn = an;
        while (points(tn,1)==y1) (tn+=1)%=points._width;

        if (y0!=y1) {
          const int
            y2 = cimg::uiround(points(tn,1)),
            x01 = x1 - x0, y01 = y1 - y0, y12 = y2 - y1,
            step = cimg::sign(y01),
            tmax = std::max(1,cimg::abs(y01)), htmax = tmax*cimg::sign(x01)/2,
            tend = tmax - (step==cimg::sign(y12));
          unsigned int y = (unsigned int)y0 - ymin;
          for (int t = 0; t<=tend; ++t, y+=step)
            if (y<Xs._height) Xs(count[y]++,y) = x0 + (t*x01 + htmax)/tmax;
        }
        go_on = nn>n;
        n = nn;
        nn = an;
      }

      cimg_pragma_openmp(parallel for cimg_openmp_if(Xs._height>=(cimg_openmp_sizefactor)*32))
      cimg_forY(Xs,y) {
        const CImg<intT> Xsy = Xs.get_shared_points(0,count[y] - 1,y).sort();
        int px = width();
        for (unsigned int k = 0; k<Xsy._width; k+=2) {
          int x0 = Xsy[k];
          const int x1 = Xsy[k + 1];
          x0+=x0==px;
          cimg_draw_scanline(x0,x1,y + ymin,color,opacity,1);
          px = x1;
        }
      }
      return *this;