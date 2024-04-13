
    CImg<intT> _autocrop(const T& value, const char axis) const {
      CImg<intT> res;
      switch (cimg::lowercase(axis)) {
      case 'x' : {
        int x0 = -1, x1 = -1;
        cimg_forX(*this,x) cimg_forYZC(*this,y,z,c)
          if ((*this)(x,y,z,c)!=value) { x0 = x; x = width(); y = height(); z = depth(); c = spectrum(); }
	if (x0>=0) {
          for (int x = width() - 1; x>=0; --x) cimg_forYZC(*this,y,z,c)
            if ((*this)(x,y,z,c)!=value) { x1 = x; x = 0; y = height(); z = depth(); c = spectrum(); }
        }
	res = CImg<intT>::vector(x0,x1);
      } break;
      case 'y' : {
        int y0 = -1, y1 = -1;
        cimg_forY(*this,y) cimg_forXZC(*this,x,z,c)
          if ((*this)(x,y,z,c)!=value) { y0 = y; x = width(); y = height(); z = depth(); c = spectrum(); }
	if (y0>=0) {
          for (int y = height() - 1; y>=0; --y) cimg_forXZC(*this,x,z,c)
            if ((*this)(x,y,z,c)!=value) { y1 = y; x = width(); y = 0; z = depth(); c = spectrum(); }
        }
  	res = CImg<intT>::vector(y0,y1);
      } break;
      case 'z' : {
        int z0 = -1, z1 = -1;
        cimg_forZ(*this,z) cimg_forXYC(*this,x,y,c)
          if ((*this)(x,y,z,c)!=value) { z0 = z; x = width(); y = height(); z = depth(); c = spectrum(); }
	if (z0>=0) {
          for (int z = depth() - 1; z>=0; --z) cimg_forXYC(*this,x,y,c)
            if ((*this)(x,y,z,c)!=value) { z1 = z; x = width(); y = height(); z = 0; c = spectrum(); }
        }
  	res = CImg<intT>::vector(z0,z1);
      } break;
      default : {
        int c0 = -1, c1 = -1;
        cimg_forC(*this,c) cimg_forXYZ(*this,x,y,z)
          if ((*this)(x,y,z,c)!=value) { c0 = c; x = width(); y = height(); z = depth(); c = spectrum(); }
	if (c0>=0) {
          for (int c = spectrum() - 1; c>=0; --c) cimg_forXYZ(*this,x,y,z)
            if ((*this)(x,y,z,c)!=value) { c1 = c; x = width(); y = height(); z = depth(); c = 0; }
        }
  	res = CImg<intT>::vector(c0,c1);
      }
      }
      return res;