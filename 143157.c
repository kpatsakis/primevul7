      \param last_frame Index of the last frame to read (can be higher than the actual number of frames, e.g. '~0U').
      \param step_frame Step value for frame reading.
      \note If step_frame==0, the current video stream is forced to be released (without any frames read).
    **/
    CImgList<T>& load_video(const char *const filename,
                            const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                            const unsigned int step_frame=1) {
#ifndef cimg_use_opencv
      if (first_frame || last_frame!=~0U || step_frame>1)
        throw CImgArgumentException(_cimglist_instance
                                    "load_video() : File '%s', arguments 'first_frame', 'last_frame' "
                                    "and 'step_frame' can be only set when using OpenCV "
                                    "(-Dcimg_use_opencv must be enabled).",
                                    cimglist_instance,filename);
      return load_ffmpeg_external(filename);
#else
      static cv::VideoCapture *captures[32] = { 0 };
      static CImgList<charT> filenames(32);
      static CImg<uintT> positions(32,1,1,1,0);
      static int last_used_index = -1;

      // Detect if a video capture already exists for the specified filename.
      cimg::mutex(9);
      int index = -1;
      if (filename) {
        if (last_used_index>=0 && !std::strcmp(filename,filenames[last_used_index])) {
          index = last_used_index;
        } else cimglist_for(filenames,l) if (filenames[l] && !std::strcmp(filename,filenames[l])) {
            index = l; break;
          }
      } else index = last_used_index;
      cimg::mutex(9,0);

      // Release stream if needed.
      if (!step_frame || (index>=0 && positions[index]>first_frame)) {
        if (index>=0) {
          cimg::mutex(9);
          captures[index]->release();
          delete captures[index];
          captures[index] = 0;
          positions[index] = 0;
          filenames[index].assign();
          if (last_used_index==index) last_used_index = -1;
          index = -1;
          cimg::mutex(9,0);
        } else
          if (filename)
            cimg::warn(_cimglist_instance
                       "load_video() : File '%s', no opened video stream associated with filename found.",
                       cimglist_instance,filename);
          else
            cimg::warn(_cimglist_instance
                       "load_video() : No opened video stream found.",
                       cimglist_instance,filename);
        if (!step_frame) return *this;
      }

      // Find empty slot for capturing video stream.
      if (index<0) {
        if (!filename)
          throw CImgArgumentException(_cimglist_instance
                                      "load_video(): No already open video reader found. You must specify a "
                                      "non-(null) filename argument for the first call.",
                                      cimglist_instance);
        else { cimg::mutex(9); cimglist_for(filenames,l) if (!filenames[l]) { index = l; break; } cimg::mutex(9,0); }
        if (index<0)
          throw CImgIOException(_cimglist_instance
                                "load_video(): File '%s', no video reader slots available. "
                                "You have to release some of your previously opened videos.",
                                cimglist_instance,filename);
        cimg::mutex(9);
        captures[index] = new cv::VideoCapture(filename);
        positions[index] = 0;
        if (!captures[index]->isOpened()) {
          delete captures[index];
          captures[index] = 0;
          cimg::mutex(9,0);
          cimg::fclose(cimg::fopen(filename,"rb"));  // Check file availability
          throw CImgIOException(_cimglist_instance
                                "load_video(): File '%s', unable to detect format of video file.",
                                cimglist_instance,filename);
        }
        CImg<charT>::string(filename).move_to(filenames[index]);
        cimg::mutex(9,0);
      }

      cimg::mutex(9);
      const unsigned int nb_frames = (unsigned int)std::max(0.,captures[index]->get(_cimg_cap_prop_frame_count));
      cimg::mutex(9,0);
      assign();

      // Skip frames if requested.
      bool go_on = true;
      unsigned int &pos = positions[index];
      while (pos<first_frame) {
        cimg::mutex(9);
        if (!captures[index]->grab()) { cimg::mutex(9,0); go_on = false; break; }
        cimg::mutex(9,0);
        ++pos;
      }

      // Read and convert frames.
      const unsigned int _last_frame = std::min(nb_frames?nb_frames - 1:~0U,last_frame);
      while (go_on && pos<=_last_frame) {
        cv::Mat cvimg;
        cimg::mutex(9);
        if (captures[index]->read(cvimg)) { CImg<T>::_cvmat2cimg(cvimg).move_to(*this); ++pos; }
        else go_on = false;
        cimg::mutex(9,0);
        if (go_on)
          for (unsigned int i = 1; go_on && i<step_frame && pos<=_last_frame; ++i, ++pos) {
            cimg::mutex(9);
            if (!captures[index]->grab()) go_on = false;
            cimg::mutex(9,0);
          }
      }

      if (!go_on || (nb_frames && pos>=nb_frames)) { // Close video stream when necessary
        cimg::mutex(9);
        captures[index]->release();
        delete captures[index];
        captures[index] = 0;
        filenames[index].assign();
        positions[index] = 0;
        index = -1;
        cimg::mutex(9,0);
      }

      cimg::mutex(9);
      last_used_index = index;
      cimg::mutex(9,0);

      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_video(): File '%s', unable to locate frame %u.",