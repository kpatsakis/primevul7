
    unsigned int _save_pandore_header_length(unsigned int id, unsigned int *dims, const unsigned int colorspace) const {
      unsigned int nbdims = 0;
      if (id==2 || id==3 || id==4) {
        dims[0] = 1; dims[1] = _width; nbdims = 2;
      }
      if (id==5 || id==6 || id==7) {
        dims[0] = 1; dims[1] = _height; dims[2] = _width; nbdims=3;
      }
      if (id==8 || id==9 || id==10) {
        dims[0] = _spectrum; dims[1] = _depth; dims[2] = _height; dims[3] = _width; nbdims = 4;
      }
      if (id==16 || id==17 || id==18) {
        dims[0] = 3; dims[1] = _height; dims[2] = _width; dims[3] = colorspace; nbdims = 4;
      }
      if (id==19 || id==20 || id==21) {
        dims[0] = 3; dims[1] = _depth; dims[2] = _height; dims[3] = _width; dims[4] = colorspace; nbdims = 5;
      }
      if (id==22 || id==23 || id==25) {
        dims[0] = _spectrum; dims[1] = _width; nbdims = 2;
      }
      if (id==26 || id==27 || id==29) {
        dims[0] = _spectrum; dims[1] = _height; dims[2] = _width; nbdims=3;
      }
      if (id==30 || id==31 || id==33) {
        dims[0] = _spectrum; dims[1] = _depth; dims[2] = _height; dims[3] = _width; nbdims = 4;
      }
      return nbdims;