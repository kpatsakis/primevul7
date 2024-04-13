        \param step_frame Step applied between each frame.
        \param[out] voxel_size Voxel size, as stored in the filename.
        \param[out] description Description, as stored in the filename.
    **/
    CImgList<T>& load_tiff(const char *const filename,
			   const unsigned int first_frame=0, const unsigned int last_frame=~0U,
			   const unsigned int step_frame=1,
                           float *const voxel_size=0,
                           CImg<charT> *const description=0) {
      const unsigned int
	nfirst_frame = first_frame<last_frame?first_frame:last_frame,
	nstep_frame = step_frame?step_frame:1;
      unsigned int nlast_frame = first_frame<last_frame?last_frame:first_frame;
#ifndef cimg_use_tiff
      cimg::unused(voxel_size,description);
      if (nfirst_frame || nlast_frame!=~0U || nstep_frame!=1)
        throw CImgArgumentException(_cimglist_instance
                                    "load_tiff(): Unable to load sub-images from file '%s' unless libtiff is enabled.",
                                    cimglist_instance,
                                    filename);

      return assign(CImg<T>::get_load_tiff(filename));
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
          cimg::warn(_cimglist_instance
                     "load_tiff(): Invalid specified frame range is [%u,%u] (step %u) since "
                     "file '%s' contains %u image(s).",
                     cimglist_instance,
                     nfirst_frame,nlast_frame,nstep_frame,filename,nb_images);

        if (nfirst_frame>=nb_images) return assign();
        if (nlast_frame>=nb_images) nlast_frame = nb_images - 1;
        assign(1 + (nlast_frame - nfirst_frame)/nstep_frame);
        TIFFSetDirectory(tif,0);
        cimglist_for(*this,l) _data[l]._load_tiff(tif,nfirst_frame + l*nstep_frame,voxel_size,description);
        TIFFClose(tif);
      } else throw CImgIOException(_cimglist_instance
                                   "load_tiff(): Failed to open file '%s'.",
                                   cimglist_instance,