    template<typename tc>
    CImg<T>& _draw_ellipse(const int x0, const int y0, const float radius1, const float radius2, const float angle,
                           const tc *const color, const float opacity,
                           const unsigned int pattern, const bool is_filled) {
      if (is_empty() || (!is_filled && !pattern)) return *this;
      const float radiusM = std::max(radius1,radius2);
      if (radius1<0 || radius2<0 || x0 - radiusM>=width() || y0 + radiusM<0 || y0 - radiusM>=height()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_ellipse(): Specified color is (null).",
                                    cimg_instance);
      const int iradius1 = (int)cimg::round(radius1), iradius2 = (int)cimg::round(radius2);
      if (!iradius1 && !iradius2) return draw_point(x0,y0,color,opacity);
      if (iradius1==iradius2) {
        if (is_filled) return draw_circle(x0,y0,iradius1,color,opacity);
        else return draw_circle(x0,y0,iradius1,color,opacity,pattern);
      }
      const float ang = (float)(angle*cimg::PI/180);

      if (!is_filled) { // Outlined
        const float ca = std::cos(ang), sa = std::sin(ang);
        CImg<int> points((unsigned int)cimg::round(6*radiusM),2);
        cimg_forX(points,k) {
          const float
            _ang = (float)(2*cimg::PI*k/points._width),
            X = (float)(radius1*std::cos(_ang)),
            Y = (float)(radius2*std::sin(_ang));
          points(k,0) = (int)cimg::round(x0 + (X*ca - Y*sa));
          points(k,1) = (int)cimg::round(y0 + (X*sa + Y*ca));
        }
        draw_polygon(points,color,opacity,pattern);
      } else { // Filled
        cimg_init_scanline(opacity);
        const float
          ca = std::cos(ang),
          sa = -std::sin(ang),
          ca2 = ca*ca,
          sa2 = sa*sa,
          casa = ca*sa,
          i1 = 1/cimg::sqr(radius1),
          i2 = 1/cimg::sqr(radius2),
          t1 = i1*ca2 + i2*sa2,
          t2 = (i2 - i1)*casa,
          t3 = i2*ca2 + i1*sa2,
          t12 = t1*2;
        const int
          _ymin = (int)std::floor(y0 - radiusM),
          _ymax = (int)std::ceil(y0 + radiusM),
          ymin = _ymin<0?0:_ymin,
          ymax = _ymax>=height()?height() - 1:_ymax;
        for (int y = ymin; y<=ymax; ++y) {
          const float
            Y = y - y0 + 0.5f,
            B = 2*t2*Y,
            C = t3*Y*Y - 1,
            D = B*B - 4*t1*C;
          if (D>=0) {
            const float sD = std::sqrt(D);
            const int
              xmin = (int)(x0 + cimg::round((-B - sD)/t12)),
              xmax = (int)(x0 + cimg::round((-B + sD)/t12));
            cimg_draw_scanline(xmin,xmax,y,color,opacity,1);
          }
        }
      }
      return *this;