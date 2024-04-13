    /**
      \param title Label set to the information displayed.
      \param display_stats Tells if image statistics must be computed and displayed.
    **/
    const CImgList<T>& print(const char *const title=0, const bool display_stats=true) const {
      unsigned int msiz = 0;
      cimglist_for(*this,l) msiz+=_data[l].size();
      msiz*=sizeof(T);
      const unsigned int mdisp = msiz<8*1024?0U:msiz<8*1024*1024?1U:2U;
      CImg<charT> _title(64);
      if (!title) cimg_snprintf(_title,_title._width,"CImgList<%s>",pixel_type());
      std::fprintf(cimg::output(),"%s%s%s%s: %sthis%s = %p, %ssize%s = %u/%u [%u %s], %sdata%s = (CImg<%s>*)%p",
                   cimg::t_magenta,cimg::t_bold,title?title:_title._data,cimg::t_normal,
                   cimg::t_bold,cimg::t_normal,(void*)this,
                   cimg::t_bold,cimg::t_normal,_width,_allocated_width,
                   mdisp==0?msiz:(mdisp==1?(msiz>>10):(msiz>>20)),
                   mdisp==0?"b":(mdisp==1?"Kio":"Mio"),
                   cimg::t_bold,cimg::t_normal,pixel_type(),(void*)begin());
      if (_data) std::fprintf(cimg::output(),"..%p.\n",(void*)((char*)end() - 1));
      else std::fprintf(cimg::output(),".\n");

      char tmp[16] = { 0 };
      cimglist_for(*this,ll) {
        cimg_snprintf(tmp,sizeof(tmp),"[%d]",ll);
        std::fprintf(cimg::output(),"  ");
        _data[ll].print(tmp,display_stats);
        if (ll==3 && width()>8) { ll = width() - 5; std::fprintf(cimg::output(),"  ...\n"); }