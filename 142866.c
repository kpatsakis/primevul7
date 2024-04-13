    template<typename t>
    void _load_tiff_contig(TIFF *const tif, const uint16 samplesperpixel, const uint32 nx, const uint32 ny) {
      t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
      if (buf) {
        uint32 row, rowsperstrip = (uint32)-1;
        TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
        for (row = 0; row<ny; row+= rowsperstrip) {
          uint32 nrow = (row + rowsperstrip>ny?ny - row:rowsperstrip);
          tstrip_t strip = TIFFComputeStrip(tif, row, 0);
          if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
            _TIFFfree(buf); TIFFClose(tif);
            throw CImgIOException(_cimg_instance
                                  "load_tiff(): Invalid strip in file '%s'.",
                                  cimg_instance,
                                  TIFFFileName(tif));
          }
          const t *ptr = buf;
          for (unsigned int rr = 0; rr<nrow; ++rr)
            for (unsigned int cc = 0; cc<nx; ++cc)
              for (unsigned int vv = 0; vv<samplesperpixel; ++vv) (*this)(cc,row + rr,vv) = (T)*(ptr++);
        }
        _TIFFfree(buf);
      }