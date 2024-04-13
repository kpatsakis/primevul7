                                 CImgDisplay::_fitscreen(dx,dy,dz,128,-85,true)
    static unsigned int _fitscreen(const unsigned int dx, const unsigned int dy, const unsigned int dz,
                                   const int dmin, const int dmax, const bool return_y) {
      const int
        u = CImgDisplay::screen_width(),
        v = CImgDisplay::screen_height();
      const float
        mw = dmin<0?cimg::round(u*-dmin/100.f):(float)dmin,
        mh = dmin<0?cimg::round(v*-dmin/100.f):(float)dmin,
        Mw = dmax<0?cimg::round(u*-dmax/100.f):(float)dmax,
        Mh = dmax<0?cimg::round(v*-dmax/100.f):(float)dmax;
      float
        w = (float)std::max(1U,dx),
        h = (float)std::max(1U,dy);
      if (dz>1) { w+=dz; h+=dz; }
      if (w<mw) { h = h*mw/w; w = mw; }
      if (h<mh) { w = w*mh/h; h = mh; }
      if (w>Mw) { h = h*Mw/w; w = Mw; }
      if (h>Mh) { w = w*Mh/h; h = Mh; }
      if (w<mw) w = mw;
      if (h<mh) h = mh;
      return std::max(1U,(unsigned int)cimg::round(return_y?h:w));