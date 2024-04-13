    //! Load a list from a PAR/REC (Philips) file.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_parrec(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load_parrec(): Specified filename is (null).",
                                    cimglist_instance);

      CImg<charT> body(1024), filenamepar(1024), filenamerec(1024);
      *body = *filenamepar = *filenamerec = 0;
      const char *const ext = cimg::split_filename(filename,body);
      if (!std::strcmp(ext,"par")) {
        std::strncpy(filenamepar,filename,filenamepar._width - 1);
        cimg_snprintf(filenamerec,filenamerec._width,"%s.rec",body._data);
      }
      if (!std::strcmp(ext,"PAR")) {
        std::strncpy(filenamepar,filename,filenamepar._width - 1);
        cimg_snprintf(filenamerec,filenamerec._width,"%s.REC",body._data);
      }
      if (!std::strcmp(ext,"rec")) {
        std::strncpy(filenamerec,filename,filenamerec._width - 1);
        cimg_snprintf(filenamepar,filenamepar._width,"%s.par",body._data);
      }
      if (!std::strcmp(ext,"REC")) {
        std::strncpy(filenamerec,filename,filenamerec._width - 1);
        cimg_snprintf(filenamepar,filenamepar._width,"%s.PAR",body._data);
      }
      std::FILE *file = cimg::fopen(filenamepar,"r");

      // Parse header file
      CImgList<floatT> st_slices;
      CImgList<uintT> st_global;
      CImg<charT> line(256); *line = 0;
      int err;
      do { err = std::fscanf(file,"%255[^\n]%*c",line._data); } while (err!=EOF && (*line=='#' || *line=='.'));
      do {
        unsigned int sn,size_x,size_y,pixsize;
        float rs,ri,ss;
        err = std::fscanf(file,"%u%*u%*u%*u%*u%*u%*u%u%*u%u%u%g%g%g%*[^\n]",&sn,&pixsize,&size_x,&size_y,&ri,&rs,&ss);
        if (err==7) {
          CImg<floatT>::vector((float)sn,(float)pixsize,(float)size_x,(float)size_y,ri,rs,ss,0).move_to(st_slices);
          unsigned int i; for (i = 0; i<st_global._width && sn<=st_global[i][2]; ++i) {}
          if (i==st_global._width) CImg<uintT>::vector(size_x,size_y,sn).move_to(st_global);
          else {
            CImg<uintT> &vec = st_global[i];
            if (size_x>vec[0]) vec[0] = size_x;
            if (size_y>vec[1]) vec[1] = size_y;
            vec[2] = sn;
          }
          st_slices[st_slices._width - 1][7] = (float)i;
        }
      } while (err==7);

      // Read data
      std::FILE *file2 = cimg::fopen(filenamerec,"rb");
      cimglist_for(st_global,l) {
        const CImg<uintT>& vec = st_global[l];
        CImg<T>(vec[0],vec[1],vec[2]).move_to(*this);
      }

      cimglist_for(st_slices,l) {
        const CImg<floatT>& vec = st_slices[l];
        const unsigned int
          sn = (unsigned int)vec[0] - 1,
          pixsize = (unsigned int)vec[1],
          size_x = (unsigned int)vec[2],
          size_y = (unsigned int)vec[3],
          imn = (unsigned int)vec[7];
        const float ri = vec[4], rs = vec[5], ss = vec[6];
        switch (pixsize) {
        case 8 : {
          CImg<ucharT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        case 16 : {
          CImg<ushortT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        case 32 : {
          CImg<uintT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        default :
          cimg::fclose(file);
          cimg::fclose(file2);
          throw CImgIOException(_cimglist_instance
                                "load_parrec(): Unsupported %d-bits pixel type for file '%s'.",
                                cimglist_instance,
                                pixsize,filename);
        }
      }
      cimg::fclose(file);
      cimg::fclose(file2);
      if (!_width)
        throw CImgIOException(_cimglist_instance
                              "load_parrec(): Failed to recognize valid PAR-REC data in file '%s'.",