    **/
    CImg<T>& load_exr(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_exr(): Specified filename is (null).",
                                    cimg_instance);
#if defined(cimg_use_openexr)
      Imf::RgbaInputFile file(filename);
      Imath::Box2i dw = file.dataWindow();
      const int
        inwidth = dw.max.x - dw.min.x + 1,
        inheight = dw.max.y - dw.min.y + 1;
      Imf::Array2D<Imf::Rgba> pixels;
      pixels.resizeErase(inheight,inwidth);
      file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y*inwidth, 1, inwidth);
      file.readPixels(dw.min.y, dw.max.y);
      assign(inwidth,inheight,1,4);
      T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
      cimg_forXY(*this,x,y) {
        *(ptr_r++) = (T)pixels[y][x].r;
        *(ptr_g++) = (T)pixels[y][x].g;
        *(ptr_b++) = (T)pixels[y][x].b;
        *(ptr_a++) = (T)pixels[y][x].a;
      }
#elif defined(cimg_use_tinyexr)
      float *res;
      const char *err = 0;
      int width = 0, height = 0;
      const int ret = LoadEXR(&res,&width,&height,filename,&err);
      if (ret) throw CImgIOException(_cimg_instance
                                     "load_exr(): Unable to load EXR file '%s'.",
                                     cimg_instance,filename);
      CImg<floatT>(out,4,width,height,1,true).get_permute_axes("yzcx").move_to(*this);
      std::free(res);
#else
      return load_other(filename);
#endif
      return *this;