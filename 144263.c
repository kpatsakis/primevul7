    const CImgList<T>& save_cimg(std::FILE *file, const bool is_compressed=false) const {
      return _save_cimg(file,0,is_compressed);
    }

    const CImgList<T>& _save_cimg(std::FILE *const file, const char *const filename,
                                 const unsigned int n0,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0) const {
#define _cimg_save_cimg_case(Ts,Tss) \
      if (!saved && !cimg::strcasecmp(Ts,str_pixeltype)) { \
        for (unsigned int l = 0; l<lmax; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n') tmp[j++]=(char)i; tmp[j] = 0; \
          W = H = D = C = 0; \
          if (cimg_sscanf(tmp,"%u %u %u %u",&W,&H,&D,&C)!=4) \
            throw CImgIOException(_cimglist_instance \
                                  "save_cimg(): Invalid size (%u,%u,%u,%u) of image[%u], for file '%s'.", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:"(FILE*)"); \
          if (W*H*D*C>0) { \
            if (l<n0 || x0>=W || y0>=H || z0>=D || c0>=D) cimg::fseek(nfile,W*H*D*C*sizeof(Tss),SEEK_CUR); \
            else { \
              const CImg<T>& img = (*this)[l - n0]; \
              const T *ptrs = img._data; \
              const unsigned int \
                x1 = x0 + img._width - 1, \
                y1 = y0 + img._height - 1, \
                z1 = z0 + img._depth - 1, \
                c1 = c0 + img._spectrum - 1, \
                nx1 = x1>=W?W - 1:x1, \
                ny1 = y1>=H?H - 1:y1, \
                nz1 = z1>=D?D - 1:z1, \
                nc1 = c1>=C?C - 1:c1; \
              CImg<Tss> raw(1 + nx1 - x0); \
              const unsigned int skipvb = c0*W*H*D*sizeof(Tss); \
              if (skipvb) cimg::fseek(nfile,skipvb,SEEK_CUR); \
              for (unsigned int v = 1 + nc1 - c0; v; --v) { \
                const unsigned int skipzb = z0*W*H*sizeof(Tss); \
                if (skipzb) cimg::fseek(nfile,skipzb,SEEK_CUR); \
                for (unsigned int z = 1 + nz1 - z0; z; --z) { \
                  const unsigned int skipyb = y0*W*sizeof(Tss); \
                  if (skipyb) cimg::fseek(nfile,skipyb,SEEK_CUR); \
                  for (unsigned int y = 1 + ny1 - y0; y; --y) { \
                    const unsigned int skipxb = x0*sizeof(Tss); \
                    if (skipxb) cimg::fseek(nfile,skipxb,SEEK_CUR); \
                    raw.assign(ptrs, raw._width); \
                    ptrs+=img._width; \
                    if (endian) cimg::invert_endianness(raw._data,raw._width); \
                    cimg::fwrite(raw._data,raw._width,nfile); \
                    const unsigned int skipxe = (W - 1 - nx1)*sizeof(Tss); \
                    if (skipxe) cimg::fseek(nfile,skipxe,SEEK_CUR); \
                  } \
                  const unsigned int skipye = (H - 1 - ny1)*W*sizeof(Tss); \
                  if (skipye) cimg::fseek(nfile,skipye,SEEK_CUR); \
                } \
                const unsigned int skipze = (D - 1 - nz1)*W*H*sizeof(Tss); \
                if (skipze) cimg::fseek(nfile,skipze,SEEK_CUR); \
              } \
              const unsigned int skipve = (C - 1 - nc1)*W*H*D*sizeof(Tss); \
              if (skipve) cimg::fseek(nfile,skipve,SEEK_CUR); \
            } \
          } \
        } \
        saved = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_cimg(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "save_cimg(): Empty instance, for file '%s'.",
                                    cimglist_instance,
                                    filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb+");
      bool saved = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N, W, H, D, C;
      int i, err;
      j = 0; while ((i=std::fgetc(nfile))!='\n' && i!=EOF && j<256) tmp[j++] = (char)i; tmp[j] = 0;
      err = cimg_sscanf(tmp,"%u%*c%255[A-Za-z64_]%*c%255[sA-Za-z_ ]",&N,str_pixeltype._data,str_endian._data);
      if (err<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "save_cimg(): CImg header not found in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      }
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      const unsigned int lmax = std::min(N,n0 + _width);
      _cimg_save_cimg_case("bool",bool);
      _cimg_save_cimg_case("unsigned_char",unsigned char);
      _cimg_save_cimg_case("uchar",unsigned char);
      _cimg_save_cimg_case("char",char);
      _cimg_save_cimg_case("unsigned_short",unsigned short);
      _cimg_save_cimg_case("ushort",unsigned short);
      _cimg_save_cimg_case("short",short);
      _cimg_save_cimg_case("unsigned_int",unsigned int);
      _cimg_save_cimg_case("uint",unsigned int);
      _cimg_save_cimg_case("int",int);
      _cimg_save_cimg_case("unsigned_int64",uint64T);
      _cimg_save_cimg_case("uint64",uint64T);
      _cimg_save_cimg_case("int64",int64T);
      _cimg_save_cimg_case("float",float);
      _cimg_save_cimg_case("double",double);
      if (!saved) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "save_cimg(): Unsupported data type '%s' for file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)",str_pixeltype._data);