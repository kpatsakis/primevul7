    template<typename t>
    void _load_tiff_tiled_separate(TIFF *const tif, const uint16 samplesperpixel,
                                   const uint32 nx, const uint32 ny, const uint32 tw, const uint32 th) {
      t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
      if (buf) {
        for (unsigned int vv = 0; vv<samplesperpixel; ++vv)
          for (unsigned int row = 0; row<ny; row+=th)
            for (unsigned int col = 0; col<nx; col+=tw) {
              if (TIFFReadTile(tif,buf,col,row,0,vv)<0) {
                _TIFFfree(buf); TIFFClose(tif);
                throw CImgIOException(_cimg_instance
                                      "load_tiff(): Invalid tile in file '%s'.",
                                      cimg_instance,
                                      TIFFFileName(tif));
              }
              const t *ptr = buf;
              for (unsigned int rr = row; rr<std::min((unsigned int)(row + th),(unsigned int)ny); ++rr)
                for (unsigned int cc = col; cc<std::min((unsigned int)(col + tw),(unsigned int)nx); ++cc)
                  (*this)(cc,rr,vv) = (T)*(ptr++);
            }
        _TIFFfree(buf);
      }