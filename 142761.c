    static CImgList<T> get_load_cimg(std::FILE *const file) {
      return CImgList<T>().load_cimg(file);
    }

    CImgList<T>& _load_cimg(std::FILE *const file, const char *const filename) {
#ifdef cimg_use_zlib
#define _cimgz_load_cimg_case(Tss) { \
   Bytef *const cbuf = new Bytef[csiz]; \
   cimg::fread(cbuf,csiz,nfile); \
   raw.assign(W,H,D,C); \
   uLongf destlen = (ulongT)raw.size()*sizeof(Tss); \
   uncompress((Bytef*)raw._data,&destlen,cbuf,csiz); \
   delete[] cbuf; \
   if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
   raw.move_to(img); \
}
#else
#define _cimgz_load_cimg_case(Tss) \
   throw CImgIOException(_cimglist_instance \
                         "load_cimg(): Unable to load compressed data from file '%s' unless zlib is enabled.", \
                         cimglist_instance, \
                         filename?filename:"(FILE*)");
#endif

#define _cimg_load_cimg_case(Ts,Tss) \
      if (!loaded && !cimg::strcasecmp(Ts,str_pixeltype)) { \
        for (unsigned int l = 0; l<N; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0 && j<255) tmp[j++] = (char)i; tmp[j] = 0; \
          W = H = D = C = 0; csiz = 0; \
          if ((err = cimg_sscanf(tmp,"%u %u %u %u #%lu",&W,&H,&D,&C,&csiz))<4) \
            throw CImgIOException(_cimglist_instance \
                                  "load_cimg(): Invalid specified size (%u,%u,%u,%u) of image %u in file '%s'.", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:("(FILE*)")); \
          if (W*H*D*C>0) { \
            CImg<Tss> raw; \
            CImg<T> &img = _data[l]; \
            if (err==5) _cimgz_load_cimg_case(Tss) \
            else { \
              img.assign(W,H,D,C); \
              T *ptrd = img._data; \
              for (ulongT to_read = img.size(); to_read; ) { \
                raw.assign((unsigned int)std::min(to_read,cimg_iobuffer)); \
                cimg::fread(raw._data,raw._width,nfile); \
                if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
                const Tss *ptrs = raw._data; \
                for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++); \
                to_read-=raw._width; \
              } \
            } \
          } \
        } \
        loaded = true; \
      }

      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Specified filename is (null).",
                                    cimglist_instance);

      const ulongT cimg_iobuffer = (ulongT)24*1024*1024;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool loaded = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N = 0, W, H, D, C;
      unsigned long csiz;
      int i, err;
      do {
        j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0 && j<255) tmp[j++] = (char)i; tmp[j] = 0;
      } while (*tmp=='#' && i>=0);
      err = cimg_sscanf(tmp,"%u%*c%255[A-Za-z64_]%*c%255[sA-Za-z_ ]",
                        &N,str_pixeltype._data,str_endian._data);
      if (err<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): CImg header not found in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      }
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      assign(N);
      _cimg_load_cimg_case("bool",bool);
      _cimg_load_cimg_case("unsigned_char",unsigned char);
      _cimg_load_cimg_case("uchar",unsigned char);
      _cimg_load_cimg_case("char",char);
      _cimg_load_cimg_case("unsigned_short",unsigned short);
      _cimg_load_cimg_case("ushort",unsigned short);
      _cimg_load_cimg_case("short",short);
      _cimg_load_cimg_case("unsigned_int",unsigned int);
      _cimg_load_cimg_case("uint",unsigned int);
      _cimg_load_cimg_case("int",int);
      _cimg_load_cimg_case("unsigned_long",ulongT);
      _cimg_load_cimg_case("ulong",ulongT);
      _cimg_load_cimg_case("long",longT);
      _cimg_load_cimg_case("unsigned_int64",uint64T);
      _cimg_load_cimg_case("uint64",uint64T);
      _cimg_load_cimg_case("int64",int64T);
      _cimg_load_cimg_case("float",float);
      _cimg_load_cimg_case("double",double);

      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Unsupported pixel type '%s' for file '%s'.",
                              cimglist_instance,
                              str_pixeltype._data,filename?filename:"(FILE*)");