    **/
    const CImg<T>& save_analyze(const char *const filename, const float *const voxel_size=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_analyze(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      std::FILE *file;
      CImg<charT> hname(1024), iname(1024);
      const char *const ext = cimg::split_filename(filename);
      short datatype = -1;
      if (!*ext) {
        cimg_snprintf(hname,hname._width,"%s.hdr",filename);
        cimg_snprintf(iname,iname._width,"%s.img",filename);
      }
      if (!cimg::strncasecmp(ext,"hdr",3)) {
        std::strcpy(hname,filename);
        std::strncpy(iname,filename,iname._width - 1);
        cimg_sprintf(iname._data + std::strlen(iname) - 3,"img");
      }
      if (!cimg::strncasecmp(ext,"img",3)) {
        std::strcpy(hname,filename);
        std::strncpy(iname,filename,iname._width - 1);
        cimg_sprintf(hname._data + std::strlen(iname) - 3,"hdr");
      }
      if (!cimg::strncasecmp(ext,"nii",3)) {
        std::strncpy(hname,filename,hname._width - 1); *iname = 0;
      }

      CImg<charT> header(*iname?348:352,1,1,1,0);
      int *const iheader = (int*)header._data;
      *iheader = 348;
      std::strcpy(header._data + 4,"CImg");
      std::strcpy(header._data + 14," ");
      ((short*)&(header[36]))[0] = 4096;
      ((char*)&(header[38]))[0] = 114;
      ((short*)&(header[40]))[0] = 4;
      ((short*)&(header[40]))[1] = (short)_width;
      ((short*)&(header[40]))[2] = (short)_height;
      ((short*)&(header[40]))[3] = (short)_depth;
      ((short*)&(header[40]))[4] = (short)_spectrum;
      if (!cimg::strcasecmp(pixel_type(),"bool")) datatype = 2;
      if (!cimg::strcasecmp(pixel_type(),"unsigned char")) datatype = 2;
      if (!cimg::strcasecmp(pixel_type(),"char")) datatype = 2;
      if (!cimg::strcasecmp(pixel_type(),"unsigned short")) datatype = 4;
      if (!cimg::strcasecmp(pixel_type(),"short")) datatype = 4;
      if (!cimg::strcasecmp(pixel_type(),"unsigned int")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"int")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"unsigned int64")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"int64")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"float")) datatype = 16;
      if (!cimg::strcasecmp(pixel_type(),"double")) datatype = 64;
      if (datatype<0)
        throw CImgIOException(_cimg_instance
                              "save_analyze(): Unsupported pixel type '%s' for file '%s'.",
                              cimg_instance,
                              pixel_type(),filename);

      ((short*)&(header[70]))[0] = datatype;
      ((short*)&(header[72]))[0] = sizeof(T);
      ((float*)&(header[108]))[0] = (float)(*iname?0:header.width());
      ((float*)&(header[112]))[0] = 1;
      ((float*)&(header[76]))[0] = 0;
      if (voxel_size) {
        ((float*)&(header[76]))[1] = voxel_size[0];
        ((float*)&(header[76]))[2] = voxel_size[1];
        ((float*)&(header[76]))[3] = voxel_size[2];
      } else ((float*)&(header[76]))[1] = ((float*)&(header[76]))[2] = ((float*)&(header[76]))[3] = 1;
      file = cimg::fopen(hname,"wb");
      cimg::fwrite(header._data,header.width(),file);
      if (*iname) { cimg::fclose(file); file = cimg::fopen(iname,"wb"); }
      cimg::fwrite(_data,size(),file);
      cimg::fclose(file);
      return *this;