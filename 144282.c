      \param filename Filename to write data to.
      \param number When positive, represents an index added to the filename. Otherwise, no number is added.
      \param digits Number of digits used for adding the number to the filename.
    **/
    const CImgList<T>& save(const char *const filename, const int number=-1, const unsigned int digits=6) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save(): Specified filename is (null).",
                                    cimglist_instance);
      // Do not test for empty instances, since .cimg format is able to manage empty instances.
      const bool is_stdout = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      CImg<charT> nfilename(1024);
      const char *const fn = is_stdout?filename:number>=0?cimg::number_filename(filename,number,digits,nfilename):
        filename;

#ifdef cimglist_save_plugin
      cimglist_save_plugin(fn);
#endif
#ifdef cimglist_save_plugin1
      cimglist_save_plugin1(fn);
#endif
#ifdef cimglist_save_plugin2
      cimglist_save_plugin2(fn);
#endif
#ifdef cimglist_save_plugin3
      cimglist_save_plugin3(fn);
#endif
#ifdef cimglist_save_plugin4
      cimglist_save_plugin4(fn);
#endif
#ifdef cimglist_save_plugin5
      cimglist_save_plugin5(fn);
#endif
#ifdef cimglist_save_plugin6
      cimglist_save_plugin6(fn);
#endif
#ifdef cimglist_save_plugin7
      cimglist_save_plugin7(fn);
#endif
#ifdef cimglist_save_plugin8
      cimglist_save_plugin8(fn);
#endif
      if (!cimg::strcasecmp(ext,"cimgz")) return save_cimg(fn,true);
      else if (!cimg::strcasecmp(ext,"cimg") || !*ext) return save_cimg(fn,false);
      else if (!cimg::strcasecmp(ext,"yuv")) return save_yuv(fn,444,true);
      else if (!cimg::strcasecmp(ext,"avi") ||
               !cimg::strcasecmp(ext,"mov") ||
               !cimg::strcasecmp(ext,"asf") ||
               !cimg::strcasecmp(ext,"divx") ||
               !cimg::strcasecmp(ext,"flv") ||
               !cimg::strcasecmp(ext,"mpg") ||
               !cimg::strcasecmp(ext,"m1v") ||
               !cimg::strcasecmp(ext,"m2v") ||
               !cimg::strcasecmp(ext,"m4v") ||
               !cimg::strcasecmp(ext,"mjp") ||
               !cimg::strcasecmp(ext,"mp4") ||
               !cimg::strcasecmp(ext,"mkv") ||
               !cimg::strcasecmp(ext,"mpe") ||
               !cimg::strcasecmp(ext,"movie") ||
               !cimg::strcasecmp(ext,"ogm") ||
               !cimg::strcasecmp(ext,"ogg") ||
               !cimg::strcasecmp(ext,"ogv") ||
               !cimg::strcasecmp(ext,"qt") ||
               !cimg::strcasecmp(ext,"rm") ||
               !cimg::strcasecmp(ext,"vob") ||
               !cimg::strcasecmp(ext,"wmv") ||
               !cimg::strcasecmp(ext,"xvid") ||
               !cimg::strcasecmp(ext,"mpeg")) return save_video(fn);
#ifdef cimg_use_tiff
      else if (!cimg::strcasecmp(ext,"tif") ||
          !cimg::strcasecmp(ext,"tiff")) return save_tiff(fn);
#endif
      else if (!cimg::strcasecmp(ext,"gz")) return save_gzip_external(fn);
      else {
        if (_width==1) _data[0].save(fn,-1);