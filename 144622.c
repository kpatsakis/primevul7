    template<typename tf, typename tc>
    CImg<T>& _load_off(CImgList<tf>& primitives, CImgList<tc>& colors,
                       std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_off(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"r");
      unsigned int nb_points = 0, nb_primitives = 0, nb_read = 0;
      CImg<charT> line(256); *line = 0;
      int err;

      // Skip comments, and read magic string OFF
      do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
      if (cimg::strncasecmp(line,"OFF",3) && cimg::strncasecmp(line,"COFF",4)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_off(): OFF header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
      if ((err = cimg_sscanf(line,"%u%u%*[^\n] ",&nb_points,&nb_primitives))!=2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_off(): Invalid number of vertices or primitives specified in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }

      // Read points data
      assign(nb_points,3);
      float X = 0, Y = 0, Z = 0;
      cimg_forX(*this,l) {
        do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
        if ((err = cimg_sscanf(line,"%f%f%f%*[^\n] ",&X,&Y,&Z))!=3) {
          if (!file) cimg::fclose(nfile);
          throw CImgIOException(_cimg_instance
                                "load_off(): Failed to read vertex %u/%u in file '%s'.",
                                cimg_instance,
                                l + 1,nb_points,filename?filename:"(FILE*)");
        }
        (*this)(l,0) = (T)X; (*this)(l,1) = (T)Y; (*this)(l,2) = (T)Z;
      }

      // Read primitive data
      primitives.assign();
      colors.assign();
      bool stop_flag = false;
      while (!stop_flag) {
        float c0 = 0.7f, c1 = 0.7f, c2 = 0.7f;
        unsigned int prim = 0, i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, i7 = 0;
        *line = 0;
        if ((err = std::fscanf(nfile,"%u",&prim))!=1) stop_flag = true;
        else {
          ++nb_read;
          switch (prim) {
          case 1 : {
            if ((err = std::fscanf(nfile,"%u%255[^\n] ",&i0,line._data))<2) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 2 : {
            if ((err = std::fscanf(nfile,"%u%u%255[^\n] ",&i0,&i1,line._data))<2) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 3 : {
            if ((err = std::fscanf(nfile,"%u%u%u%255[^\n] ",&i0,&i1,&i2,line._data))<3) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i2,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 4 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,line._data))<4) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 5 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,line._data))<5) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i4,i3).move_to(primitives);
              colors.insert(2,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++nb_primitives;
            }
          } break;
          case 6 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,line._data))<6) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i5,i4,i3).move_to(primitives);
              colors.insert(2,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++nb_primitives;
            }
          } break;
          case 7 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,&i6,line._data))<7) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i4,i3,i1).move_to(primitives);
              CImg<tf>::vector(i0,i6,i5,i4).move_to(primitives);
              CImg<tf>::vector(i3,i2,i1).move_to(primitives);
              colors.insert(3,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++(++nb_primitives);
            }
          } break;
          case 8 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,&i6,&i7,line._data))<7) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i5,i4,i3).move_to(primitives);
              CImg<tf>::vector(i0,i7,i6,i5).move_to(primitives);
              colors.insert(3,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++(++nb_primitives);
            }
          } break;
          default :
            cimg::warn(_cimg_instance
                       "load_off(): Failed to read primitive %u/%u (%u vertices) from file '%s'.",
                       cimg_instance,
                       nb_read,nb_primitives,prim,filename?filename:"(FILE*)");

            err = std::fscanf(nfile,"%*[^\n] ");
          }
        }
      }
      if (!file) cimg::fclose(nfile);
      if (primitives._width!=nb_primitives)
        cimg::warn(_cimg_instance
                   "load_off(): Only %u/%u primitives read from file '%s'.",
                   cimg_instance,
                   primitives._width,nb_primitives,filename?filename:"(FILE*)");
      return *this;