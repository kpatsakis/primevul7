
    const CImg<T>& _save_pandore(std::FILE *const file, const char *const filename,
                                 const unsigned int colorspace) const {

#define __cimg_save_pandore_case(dtype) \
       dtype *buffer = new dtype[size()]; \
       const T *ptrs = _data; \
       cimg_foroff(*this,off) *(buffer++) = (dtype)(*(ptrs++)); \
       buffer-=size(); \
       cimg::fwrite(buffer,size(),nfile); \
       delete[] buffer

#define _cimg_save_pandore_case(sy,sz,sv,stype,id) \
      if (!saved && (sy?(sy==_height):true) && (sz?(sz==_depth):true) && \
          (sv?(sv==_spectrum):true) && !std::strcmp(stype,pixel_type())) { \
	unsigned int *iheader = (unsigned int*)(header + 12); \
	nbdims = _save_pandore_header_length((*iheader=id),dims,colorspace); \
	cimg::fwrite(header,36,nfile); \
        if (sizeof(unsigned long)==4) { CImg<ulongT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned long)dims[d]; cimg::fwrite(ndims._data,nbdims,nfile); } \
        else if (sizeof(unsigned int)==4) { CImg<uintT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned int)dims[d]; cimg::fwrite(ndims._data,nbdims,nfile); } \
        else if (sizeof(unsigned short)==4) { CImg<ushortT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned short)dims[d]; cimg::fwrite(ndims._data,nbdims,nfile); } \
        else throw CImgIOException(_cimg_instance \
                                   "save_pandore(): Unsupported datatype for file '%s'.",\
                                   cimg_instance, \
                                   filename?filename:"(FILE*)"); \
	if (id==2 || id==5 || id==8 || id==16 || id==19 || id==22 || id==26 || id==30) { \
          __cimg_save_pandore_case(unsigned char); \
	} else if (id==3 || id==6 || id==9 || id==17 || id==20 || id==23 || id==27 || id==31) { \
          if (sizeof(unsigned long)==4) { __cimg_save_pandore_case(unsigned long); } \
          else if (sizeof(unsigned int)==4) { __cimg_save_pandore_case(unsigned int); } \
          else if (sizeof(unsigned short)==4) { __cimg_save_pandore_case(unsigned short); } \
          else throw CImgIOException(_cimg_instance \
                                     "save_pandore(): Unsupported datatype for file '%s'.",\
                                     cimg_instance, \
                                     filename?filename:"(FILE*)"); \
	} else if (id==4 || id==7 || id==10 || id==18 || id==21 || id==25 || id==29 || id==33) { \
          if (sizeof(double)==4) { __cimg_save_pandore_case(double); } \
          else if (sizeof(float)==4) { __cimg_save_pandore_case(float); } \
          else throw CImgIOException(_cimg_instance \
                                     "save_pandore(): Unsupported datatype for file '%s'.",\
                                     cimg_instance, \
                                     filename?filename:"(FILE*)"); \
        } \
	saved = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pandore(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      unsigned char header[36] = { 'P','A','N','D','O','R','E','0','4',0,0,0,
                                   0,0,0,0,'C','I','m','g',0,0,0,0,0,
                                   'N','o',' ','d','a','t','e',0,0,0,0 };
      unsigned int nbdims, dims[5] = { 0 };
      bool saved = false;
      _cimg_save_pandore_case(1,1,1,"unsigned char",2);
      _cimg_save_pandore_case(1,1,1,"char",3);
      _cimg_save_pandore_case(1,1,1,"unsigned short",3);
      _cimg_save_pandore_case(1,1,1,"short",3);
      _cimg_save_pandore_case(1,1,1,"unsigned int",3);
      _cimg_save_pandore_case(1,1,1,"int",3);
      _cimg_save_pandore_case(1,1,1,"unsigned int64",3);
      _cimg_save_pandore_case(1,1,1,"int64",3);
      _cimg_save_pandore_case(1,1,1,"float",4);
      _cimg_save_pandore_case(1,1,1,"double",4);

      _cimg_save_pandore_case(0,1,1,"unsigned char",5);
      _cimg_save_pandore_case(0,1,1,"char",6);
      _cimg_save_pandore_case(0,1,1,"unsigned short",6);
      _cimg_save_pandore_case(0,1,1,"short",6);
      _cimg_save_pandore_case(0,1,1,"unsigned int",6);
      _cimg_save_pandore_case(0,1,1,"int",6);
      _cimg_save_pandore_case(0,1,1,"unsigned int64",6);
      _cimg_save_pandore_case(0,1,1,"int64",6);
      _cimg_save_pandore_case(0,1,1,"float",7);
      _cimg_save_pandore_case(0,1,1,"double",7);

      _cimg_save_pandore_case(0,0,1,"unsigned char",8);
      _cimg_save_pandore_case(0,0,1,"char",9);
      _cimg_save_pandore_case(0,0,1,"unsigned short",9);
      _cimg_save_pandore_case(0,0,1,"short",9);
      _cimg_save_pandore_case(0,0,1,"unsigned int",9);
      _cimg_save_pandore_case(0,0,1,"int",9);
      _cimg_save_pandore_case(0,0,1,"unsigned int64",9);
      _cimg_save_pandore_case(0,0,1,"int64",9);
      _cimg_save_pandore_case(0,0,1,"float",10);
      _cimg_save_pandore_case(0,0,1,"double",10);

      _cimg_save_pandore_case(0,1,3,"unsigned char",16);
      _cimg_save_pandore_case(0,1,3,"char",17);
      _cimg_save_pandore_case(0,1,3,"unsigned short",17);
      _cimg_save_pandore_case(0,1,3,"short",17);
      _cimg_save_pandore_case(0,1,3,"unsigned int",17);
      _cimg_save_pandore_case(0,1,3,"int",17);
      _cimg_save_pandore_case(0,1,3,"unsigned int64",17);
      _cimg_save_pandore_case(0,1,3,"int64",17);
      _cimg_save_pandore_case(0,1,3,"float",18);
      _cimg_save_pandore_case(0,1,3,"double",18);

      _cimg_save_pandore_case(0,0,3,"unsigned char",19);
      _cimg_save_pandore_case(0,0,3,"char",20);
      _cimg_save_pandore_case(0,0,3,"unsigned short",20);
      _cimg_save_pandore_case(0,0,3,"short",20);
      _cimg_save_pandore_case(0,0,3,"unsigned int",20);
      _cimg_save_pandore_case(0,0,3,"int",20);
      _cimg_save_pandore_case(0,0,3,"unsigned int64",20);
      _cimg_save_pandore_case(0,0,3,"int64",20);
      _cimg_save_pandore_case(0,0,3,"float",21);
      _cimg_save_pandore_case(0,0,3,"double",21);

      _cimg_save_pandore_case(1,1,0,"unsigned char",22);
      _cimg_save_pandore_case(1,1,0,"char",23);
      _cimg_save_pandore_case(1,1,0,"unsigned short",23);
      _cimg_save_pandore_case(1,1,0,"short",23);
      _cimg_save_pandore_case(1,1,0,"unsigned int",23);
      _cimg_save_pandore_case(1,1,0,"int",23);
      _cimg_save_pandore_case(1,1,0,"unsigned int64",23);
      _cimg_save_pandore_case(1,1,0,"int64",23);
      _cimg_save_pandore_case(1,1,0,"float",25);
      _cimg_save_pandore_case(1,1,0,"double",25);

      _cimg_save_pandore_case(0,1,0,"unsigned char",26);
      _cimg_save_pandore_case(0,1,0,"char",27);
      _cimg_save_pandore_case(0,1,0,"unsigned short",27);
      _cimg_save_pandore_case(0,1,0,"short",27);
      _cimg_save_pandore_case(0,1,0,"unsigned int",27);
      _cimg_save_pandore_case(0,1,0,"int",27);
      _cimg_save_pandore_case(0,1,0,"unsigned int64",27);
      _cimg_save_pandore_case(0,1,0,"int64",27);
      _cimg_save_pandore_case(0,1,0,"float",29);
      _cimg_save_pandore_case(0,1,0,"double",29);

      _cimg_save_pandore_case(0,0,0,"unsigned char",30);
      _cimg_save_pandore_case(0,0,0,"char",31);
      _cimg_save_pandore_case(0,0,0,"unsigned short",31);
      _cimg_save_pandore_case(0,0,0,"short",31);
      _cimg_save_pandore_case(0,0,0,"unsigned int",31);
      _cimg_save_pandore_case(0,0,0,"int",31);
      _cimg_save_pandore_case(0,0,0,"unsigned int64",31);
      _cimg_save_pandore_case(0,0,0,"int64",31);
      _cimg_save_pandore_case(0,0,0,"float",33);
      _cimg_save_pandore_case(0,0,0,"double",33);

      if (!file) cimg::fclose(nfile);
      return *this;