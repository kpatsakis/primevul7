    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const int val0, const int val1, ...):
      _width(0),_allocated_width(0),_data(0) {
#define _CImgList_stdarg(t) { \
	assign(n,width,height,depth,spectrum); \
	const ulongT siz = (ulongT)width*height*depth*spectrum, nsiz = siz*n; \
	T *ptrd = _data->_data; \
	va_list ap; \
	va_start(ap,val1); \
	for (ulongT l = 0, s = 0, i = 0; i<nsiz; ++i) { \
	  *(ptrd++) = (T)(i==0?val0:(i==1?val1:va_arg(ap,t))); \
	  if ((++s)==siz) { ptrd = _data[++l]._data; s = 0; } \
	} \
	va_end(ap); \
      }
      _CImgList_stdarg(int);