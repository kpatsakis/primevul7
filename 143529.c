
    const CImg<T>& _save_inr(std::FILE *const file, const char *const filename, const float *const voxel_size) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_inr(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      int inrpixsize = -1;
      const char *inrtype = "unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0";
      if (!cimg::strcasecmp(pixel_type(),"unsigned char")) {
        inrtype = "unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0"; inrpixsize = 1;
      }
      if (!cimg::strcasecmp(pixel_type(),"char")) {
        inrtype = "fixed\nPIXSIZE=8 bits\nSCALE=2**0"; inrpixsize = 1;
      }
      if (!cimg::strcasecmp(pixel_type(),"unsigned short")) {
        inrtype = "unsigned fixed\nPIXSIZE=16 bits\nSCALE=2**0";inrpixsize = 2;
      }
      if (!cimg::strcasecmp(pixel_type(),"short")) {
        inrtype = "fixed\nPIXSIZE=16 bits\nSCALE=2**0"; inrpixsize = 2;
      }
      if (!cimg::strcasecmp(pixel_type(),"unsigned int")) {
        inrtype = "unsigned fixed\nPIXSIZE=32 bits\nSCALE=2**0";inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"int")) {
        inrtype = "fixed\nPIXSIZE=32 bits\nSCALE=2**0"; inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"float")) {
        inrtype = "float\nPIXSIZE=32 bits"; inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"double")) {
        inrtype = "float\nPIXSIZE=64 bits"; inrpixsize = 8;
      }
      if (inrpixsize<=0)
        throw CImgIOException(_cimg_instance
                              "save_inr(): Unsupported pixel type '%s' for file '%s'",
                              cimg_instance,
                              pixel_type(),filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      CImg<charT> header(257);
      int err = cimg_snprintf(header,header._width,"#INRIMAGE-4#{\nXDIM=%u\nYDIM=%u\nZDIM=%u\nVDIM=%u\n",
                              _width,_height,_depth,_spectrum);
      if (voxel_size) err+=cimg_sprintf(header._data + err,"VX=%g\nVY=%g\nVZ=%g\n",
                                        voxel_size[0],voxel_size[1],voxel_size[2]);
      err+=cimg_sprintf(header._data + err,"TYPE=%s\nCPU=%s\n",inrtype,cimg::endianness()?"sun":"decm");
      std::memset(header._data + err,'\n',252 - err);
      std::memcpy(header._data + 252,"##}\n",4);
      cimg::fwrite(header._data,256,nfile);
      cimg_forXYZ(*this,x,y,z) cimg_forC(*this,c) cimg::fwrite(&((*this)(x,y,z,c)),1,nfile);
      if (!file) cimg::fclose(nfile);
      return *this;