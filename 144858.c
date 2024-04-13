
    static void _load_inr_header(std::FILE *file, int out[8], float *const voxel_size) {
      CImg<charT> item(1024), tmp1(64), tmp2(64);
      *item = *tmp1 = *tmp2 = 0;
      out[0] = std::fscanf(file,"%63s",item._data);
      out[0] = out[1] = out[2] = out[3] = out[5] = 1; out[4] = out[6] = out[7] = -1;
      if (cimg::strncasecmp(item,"#INRIMAGE-4#{",13)!=0)
        throw CImgIOException("CImg<%s>::load_inr(): INRIMAGE-4 header not found.",
                              pixel_type());

      while (std::fscanf(file," %63[^\n]%*c",item._data)!=EOF && std::strncmp(item,"##}",3)) {
        cimg_sscanf(item," XDIM%*[^0-9]%d",out);
        cimg_sscanf(item," YDIM%*[^0-9]%d",out + 1);
        cimg_sscanf(item," ZDIM%*[^0-9]%d",out + 2);
        cimg_sscanf(item," VDIM%*[^0-9]%d",out + 3);
        cimg_sscanf(item," PIXSIZE%*[^0-9]%d",out + 6);
        if (voxel_size) {
          cimg_sscanf(item," VX%*[^0-9.+-]%f",voxel_size);
          cimg_sscanf(item," VY%*[^0-9.+-]%f",voxel_size + 1);
          cimg_sscanf(item," VZ%*[^0-9.+-]%f",voxel_size + 2);
        }
        if (cimg_sscanf(item," CPU%*[ =]%s",tmp1._data)) out[7] = cimg::strncasecmp(tmp1,"sun",3)?0:1;
        switch (cimg_sscanf(item," TYPE%*[ =]%s %s",tmp1._data,tmp2._data)) {
        case 0 : break;
        case 2 :
          out[5] = cimg::strncasecmp(tmp1,"unsigned",8)?1:0;
          std::strncpy(tmp1,tmp2,tmp1._width - 1); // fallthrough
        case 1 :
          if (!cimg::strncasecmp(tmp1,"int",3) || !cimg::strncasecmp(tmp1,"fixed",5))  out[4] = 0;
          if (!cimg::strncasecmp(tmp1,"float",5) || !cimg::strncasecmp(tmp1,"double",6)) out[4] = 1;
          if (!cimg::strncasecmp(tmp1,"packed",6)) out[4] = 2;
          if (out[4]>=0) break; // fallthrough
        default :
          throw CImgIOException("CImg<%s>::load_inr(): Invalid pixel type '%s' defined in header.",
                                pixel_type(),
                                tmp2._data);
        }
      }
      if (out[0]<0 || out[1]<0 || out[2]<0 || out[3]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Invalid dimensions (%d,%d,%d,%d) defined in header.",
                              pixel_type(),
                              out[0],out[1],out[2],out[3]);
      if (out[4]<0 || out[5]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Incomplete pixel type defined in header.",
                              pixel_type());
      if (out[6]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Incomplete PIXSIZE field defined in header.",
                              pixel_type());
      if (out[7]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Big/Little Endian coding type undefined in header.",
                              pixel_type());