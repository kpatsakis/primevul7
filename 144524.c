     **/
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const int value0, const int value1, ...):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
#define _CImg_stdarg(img,a0,a1,N,t) { \
	size_t _siz = (size_t)N; \
	if (_siz--) { \
	  va_list ap; \
	  va_start(ap,a1); \
	  T *ptrd = (img)._data; \
	  *(ptrd++) = (T)a0; \
	  if (_siz--) { \
	    *(ptrd++) = (T)a1; \
	    for ( ; _siz; --_siz) *(ptrd++) = (T)va_arg(ap,t); \
	  } \
	  va_end(ap); \
	} \
      }
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,(size_t)size_x*size_y*size_z*size_c,int);