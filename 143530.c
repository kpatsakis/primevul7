    **/
    CImg<T>& fillX(const unsigned int y, const unsigned int z, const unsigned int c, const int a0, ...) {
#define _cimg_fill1(x,y,z,c,off,siz,t) { \
    va_list ap; va_start(ap,a0); T *ptrd = data(x,y,z,c); *ptrd = (T)a0; \
    for (unsigned int k = 1; k<siz; ++k) { ptrd+=off; *ptrd = (T)va_arg(ap,t); } \
    va_end(ap); }
      if (y<_height && z<_depth && c<_spectrum) _cimg_fill1(0,y,z,c,1,_width,int);
      return *this;