                                     const unsigned int z1, const unsigned int c1) {
      return CImgList<T>().load_cimg(file,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    CImgList<T>& _load_cimg(std::FILE *const file, const char *const filename,
			    const unsigned int n0, const unsigned int n1,
			    const unsigned int x0, const unsigned int y0,
                            const unsigned int z0, const unsigned int c0,
			    const unsigned int x1, const unsigned int y1,
                            const unsigned int z1, const unsigned int c1) {
#define _cimg_load_cimg_case2(Ts,Tss) \
      if (!loaded && !cimg::strcasecmp(Ts,str_pixeltype)) { \
        for (unsigned int l = 0; l<=nn1; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0) tmp[j++] = (char)i; tmp[j] = 0; \
          W = H = D = C = 0; \
          if (cimg_sscanf(tmp,"%u %u %u %u",&W,&H,&D,&C)!=4) \
            throw CImgIOException(_cimglist_instance \
                                  "load_cimg(): Invalid specified size (%u,%u,%u,%u) of image %u in file '%s'", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:"(FILE*)"); \
          if (W*H*D*C>0) { \
            if (l<nn0 || nx0>=W || ny0>=H || nz0>=D || nc0>=C) cimg::fseek(nfile,W*H*D*C*sizeof(Tss),SEEK_CUR); \
            else { \
              const unsigned int \
                _nx1 = nx1==~0U?W - 1:nx1, \
                _ny1 = ny1==~0U?H - 1:ny1, \
                _nz1 = nz1==~0U?D - 1:nz1, \
                _nc1 = nc1==~0U?C - 1:nc1; \
              if (_nx1>=W || _ny1>=H || _nz1>=D || _nc1>=C) \
                throw CImgArgumentException(_cimglist_instance \
                                            "load_cimg(): Invalid specified coordinates " \
                                            "[%u](%u,%u,%u,%u) -> [%u](%u,%u,%u,%u) " \
                                            "because image [%u] in file '%s' has size (%u,%u,%u,%u).", \
                                            cimglist_instance, \
                                            n0,x0,y0,z0,c0,n1,x1,y1,z1,c1,l,filename?filename:"(FILE*)",W,H,D,C); \
              CImg<Tss> raw(1 + _nx1 - nx0); \
              CImg<T> &img = _data[l - nn0]; \
              img.assign(1 + _nx1 - nx0,1 + _ny1 - ny0,1 + _nz1 - nz0,1 + _nc1 - nc0); \
              T *ptrd = img._data; \
              ulongT skipvb = nc0*W*H*D*sizeof(Tss); \
              if (skipvb) cimg::fseek(nfile,skipvb,SEEK_CUR); \
              for (unsigned int c = 1 + _nc1 - nc0; c; --c) { \
                const ulongT skipzb = nz0*W*H*sizeof(Tss); \
                if (skipzb) cimg::fseek(nfile,skipzb,SEEK_CUR); \
                for (unsigned int z = 1 + _nz1 - nz0; z; --z) { \
                  const ulongT skipyb = ny0*W*sizeof(Tss); \
                  if (skipyb) cimg::fseek(nfile,skipyb,SEEK_CUR); \
                  for (unsigned int y = 1 + _ny1 - ny0; y; --y) { \
                    const ulongT skipxb = nx0*sizeof(Tss); \
                    if (skipxb) cimg::fseek(nfile,skipxb,SEEK_CUR); \
                    cimg::fread(raw._data,raw._width,nfile); \
                    if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw._width); \
                    const Tss *ptrs = raw._data; \
                    for (unsigned int off = raw._width; off; --off) *(ptrd++) = (T)*(ptrs++); \
                    const ulongT skipxe = (W - 1 - _nx1)*sizeof(Tss); \
                    if (skipxe) cimg::fseek(nfile,skipxe,SEEK_CUR); \
                  } \
                  const ulongT skipye = (H - 1 - _ny1)*W*sizeof(Tss); \
                  if (skipye) cimg::fseek(nfile,skipye,SEEK_CUR); \
                } \
                const ulongT skipze = (D - 1 - _nz1)*W*H*sizeof(Tss); \
                if (skipze) cimg::fseek(nfile,skipze,SEEK_CUR); \
              } \
              const ulongT skipve = (C - 1 - _nc1)*W*H*D*sizeof(Tss); \
              if (skipve) cimg::fseek(nfile,skipve,SEEK_CUR); \
            } \
          } \
        } \
        loaded = true; \
      }

      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Specified filename is (null).",
                                    cimglist_instance);
      unsigned int
        nn0 = std::min(n0,n1), nn1 = std::max(n0,n1),
        nx0 = std::min(x0,x1), nx1 = std::max(x0,x1),
        ny0 = std::min(y0,y1), ny1 = std::max(y0,y1),
        nz0 = std::min(z0,z1), nz1 = std::max(z0,z1),
        nc0 = std::min(c0,c1), nc1 = std::max(c0,c1);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool loaded = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N, W, H, D, C;
      int i, err;
      j = 0; while ((i=std::fgetc(nfile))!='\n' && i!=EOF && j<256) tmp[j++] = (char)i; tmp[j] = 0;
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
      nn1 = n1==~0U?N - 1:n1;
      if (nn1>=N)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Invalid specified coordinates [%u](%u,%u,%u,%u) -> [%u](%u,%u,%u,%u) "
                                    "because file '%s' contains only %u images.",
                                    cimglist_instance,
                                    n0,x0,y0,z0,c0,n1,x1,y1,z1,c1,filename?filename:"(FILE*)",N);
      assign(1 + nn1 - n0);
      _cimg_load_cimg_case2("bool",bool);
      _cimg_load_cimg_case2("unsigned_char",unsigned char);
      _cimg_load_cimg_case2("uchar",unsigned char);
      _cimg_load_cimg_case2("char",char);
      _cimg_load_cimg_case2("unsigned_short",unsigned short);
      _cimg_load_cimg_case2("ushort",unsigned short);
      _cimg_load_cimg_case2("short",short);
      _cimg_load_cimg_case2("unsigned_int",unsigned int);
      _cimg_load_cimg_case2("uint",unsigned int);
      _cimg_load_cimg_case2("int",int);
      _cimg_load_cimg_case2("unsigned_long",ulongT);
      _cimg_load_cimg_case2("ulong",ulongT);
      _cimg_load_cimg_case2("long",longT);
      _cimg_load_cimg_case2("unsigned_int64",uint64T);
      _cimg_load_cimg_case2("uint64",uint64T);
      _cimg_load_cimg_case2("int64",int64T);
      _cimg_load_cimg_case2("float",float);
      _cimg_load_cimg_case2("double",double);
      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Unsupported pixel type '%s' for file '%s'.",
                              cimglist_instance,
                              str_pixeltype._data,filename?filename:"(FILE*)");