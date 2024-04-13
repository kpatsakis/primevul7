     **/
    CImg<T>& load_tiff(const char *const filename,
		       const unsigned int first_frame=0, const unsigned int last_frame=~0U,
		       const unsigned int step_frame=1,
                       float *const voxel_size=0,
                       CImg<charT> *const description=0) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_tiff(): Specified filename is (null).",
                                    cimg_instance);

      const unsigned int
	nfirst_frame = first_frame<last_frame?first_frame:last_frame,
	nstep_frame = step_frame?step_frame:1;
      unsigned int nlast_frame = first_frame<last_frame?last_frame:first_frame;

#ifndef cimg_use_tiff
      cimg::unused(voxel_size,description);
      if (nfirst_frame || nlast_frame!=~0U || nstep_frame>1)
        throw CImgArgumentException(_cimg_instance
                                    "load_tiff(): Unable to read sub-images from file '%s' unless libtiff is enabled.",
                                    cimg_instance,
                                    filename);
      return load_other(filename);
#else
#if cimg_verbosity<3
      TIFFSetWarningHandler(0);
      TIFFSetErrorHandler(0);
#endif
      TIFF *tif = TIFFOpen(filename,"r");
      if (tif) {
        unsigned int nb_images = 0;
        do ++nb_images; while (TIFFReadDirectory(tif));
        if (nfirst_frame>=nb_images || (nlast_frame!=~0U && nlast_frame>=nb_images))
          cimg::warn(_cimg_instance
                     "load_tiff(): File '%s' contains %u image(s) while specified frame range is [%u,%u] (step %u).",
                     cimg_instance,
                     filename,nb_images,nfirst_frame,nlast_frame,nstep_frame);

        if (nfirst_frame>=nb_images) return assign();
        if (nlast_frame>=nb_images) nlast_frame = nb_images - 1;
        TIFFSetDirectory(tif,0);
        CImg<T> frame;
        for (unsigned int l = nfirst_frame; l<=nlast_frame; l+=nstep_frame) {
          frame._load_tiff(tif,l,voxel_size,description);
          if (l==nfirst_frame)
            assign(frame._width,frame._height,1 + (nlast_frame - nfirst_frame)/nstep_frame,frame._spectrum);
          if (frame._width>_width || frame._height>_height || frame._spectrum>_spectrum)
            resize(std::max(frame._width,_width),
                   std::max(frame._height,_height),-100,
                   std::max(frame._spectrum,_spectrum),0);
          draw_image(0,0,(l - nfirst_frame)/nstep_frame,frame);
        }
        TIFFClose(tif);
      } else throw CImgIOException(_cimg_instance
                                   "load_tiff(): Failed to open file '%s'.",
                                   cimg_instance,
                                   filename);
      return *this;
#endif