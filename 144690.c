    **/
    const CImg<T>& print(const char *const title=0, const bool display_stats=true) const {

      int xm = 0, ym = 0, zm = 0, vm = 0, xM = 0, yM = 0, zM = 0, vM = 0;
      CImg<doubleT> st;
      if (!is_empty() && display_stats) {
        st = get_stats();
        xm = (int)st[4]; ym = (int)st[5], zm = (int)st[6], vm = (int)st[7];
        xM = (int)st[8]; yM = (int)st[9], zM = (int)st[10], vM = (int)st[11];
      }

      const ulongT siz = size(), msiz = siz*sizeof(T), siz1 = siz - 1,
        mdisp = msiz<8*1024?0U:msiz<8*1024*1024?1U:2U, width1 = _width - 1;

      CImg<charT> _title(64);
      if (!title) cimg_snprintf(_title,_title._width,"CImg<%s>",pixel_type());

      std::fprintf(cimg::output(),"%s%s%s%s: %sthis%s = %p, %ssize%s = (%u,%u,%u,%u) [%lu %s], %sdata%s = (%s*)%p",
                   cimg::t_magenta,cimg::t_bold,title?title:_title._data,cimg::t_normal,
                   cimg::t_bold,cimg::t_normal,(void*)this,
                   cimg::t_bold,cimg::t_normal,_width,_height,_depth,_spectrum,
                   (unsigned long)(mdisp==0?msiz:(mdisp==1?(msiz>>10):(msiz>>20))),
                   mdisp==0?"b":(mdisp==1?"Kio":"Mio"),
                   cimg::t_bold,cimg::t_normal,pixel_type(),(void*)begin());
      if (_data)
        std::fprintf(cimg::output(),"..%p (%s) = [ ",(void*)((char*)end() - 1),_is_shared?"shared":"non-shared");
      else std::fprintf(cimg::output()," (%s) = [ ",_is_shared?"shared":"non-shared");

      if (!is_empty()) cimg_foroff(*this,off) {
        std::fprintf(cimg::output(),"%g",(double)_data[off]);
        if (off!=siz1) std::fprintf(cimg::output(),"%s",off%_width==width1?" ; ":" ");
        if (off==7 && siz>16) { off = siz1 - 8; std::fprintf(cimg::output(),"... "); }
      }
      if (!is_empty() && display_stats)
	std::fprintf(cimg::output(),
                     " ], %smin%s = %g, %smax%s = %g, %smean%s = %g, %sstd%s = %g, %scoords_min%s = (%u,%u,%u,%u), "
                     "%scoords_max%s = (%u,%u,%u,%u).\n",
		     cimg::t_bold,cimg::t_normal,st[0],
                     cimg::t_bold,cimg::t_normal,st[1],
                     cimg::t_bold,cimg::t_normal,st[2],
                     cimg::t_bold,cimg::t_normal,std::sqrt(st[3]),
                     cimg::t_bold,cimg::t_normal,xm,ym,zm,vm,
                     cimg::t_bold,cimg::t_normal,xM,yM,zM,vM);
      else std::fprintf(cimg::output(),"%s].\n",is_empty()?"":" ");
      std::fflush(cimg::output());
      return *this;