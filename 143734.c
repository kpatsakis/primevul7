    **/
    CImg<T>& fillZ(const unsigned int x, const unsigned int y, const unsigned int c, const int a0, ...) {
      const ulongT wh = (ulongT)_width*_height;
      if (x<_width && y<_height && c<_spectrum) _cimg_fill1(x,y,0,c,wh,_depth,int);
      return *this;