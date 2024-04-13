
    CImg<T>& _load_inr(std::FILE *const file, const char *const filename, float *const voxel_size) {
#define _cimg_load_inr_case(Tf,sign,pixsize,Ts) \
     if (!loaded && fopt[6]==pixsize && fopt[4]==Tf && fopt[5]==sign) { \
        Ts *xval, *const val = new Ts[(size_t)fopt[0]*fopt[3]]; \
        cimg_forYZ(*this,y,z) { \
            cimg::fread(val,fopt[0]*fopt[3],nfile); \
            if (fopt[7]!=endian) cimg::invert_endianness(val,fopt[0]*fopt[3]); \
            xval = val; cimg_forX(*this,x) cimg_forC(*this,c) (*this)(x,y,z,c) = (T)*(xval++); \
          } \
        delete[] val; \
        loaded = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_inr(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      int fopt[8], endian = cimg::endianness()?1:0;
      bool loaded = false;
      if (voxel_size) voxel_size[0] = voxel_size[1] = voxel_size[2] = 1;
      _load_inr_header(nfile,fopt,voxel_size);
      assign(fopt[0],fopt[1],fopt[2],fopt[3]);
      _cimg_load_inr_case(0,0,8,unsigned char);
      _cimg_load_inr_case(0,1,8,char);
      _cimg_load_inr_case(0,0,16,unsigned short);
      _cimg_load_inr_case(0,1,16,short);
      _cimg_load_inr_case(0,0,32,unsigned int);
      _cimg_load_inr_case(0,1,32,int);
      _cimg_load_inr_case(1,0,32,float);
      _cimg_load_inr_case(1,1,32,float);
      _cimg_load_inr_case(1,0,64,double);
      _cimg_load_inr_case(1,1,64,double);
      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_inr(): Unknown pixel type defined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;