    //! Load image from a TIFF file \newinstance.
    static CImg<T> get_load_tiff(const char *const filename,
				 const unsigned int first_frame=0, const unsigned int last_frame=~0U,
				 const unsigned int step_frame=1,
                                 float *const voxel_size=0,
                                 CImg<charT> *const description=0) {
      return CImg<T>().load_tiff(filename,first_frame,last_frame,step_frame,voxel_size,description);