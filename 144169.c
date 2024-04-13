    /**
       \param font_height Height of the desired font (exact match for 13,23,53,103).
       \param is_variable_width Decide if the font has a variable (\c true) or fixed (\c false) width.
    **/
    static const CImgList<ucharT>& font(const unsigned int requested_height, const bool is_variable_width=true) {
      if (!requested_height) return CImgList<ucharT>::const_empty();
      cimg::mutex(11);
      static const unsigned char font_resizemap[] = {
        0, 4, 7, 9, 11, 13, 15, 17, 19, 21, 22, 24, 26, 27, 29, 30,
        32, 33, 35, 36, 38, 39, 41, 42, 43, 45, 46, 47, 49, 50, 51, 52,
        54, 55, 56, 58, 59, 60, 61, 62, 64, 65, 66, 67, 68, 69, 71, 72,
        73, 74, 75, 76, 77, 78, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
        90, 91, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
        123, 124, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
        138, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149, 150, 151,
        152, 153, 154, 155, 156, 157, 157, 158, 159, 160, 161, 162, 163, 164, 164, 165,
        166, 167, 168, 169, 170, 170, 171, 172, 173, 174, 175, 176, 176, 177, 178, 179,
        180, 181, 181, 182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 191, 192,
        193, 194, 195, 196, 196, 197, 198, 199, 200, 200, 201, 202, 203, 204, 205, 205,
        206, 207, 208, 209, 209, 210, 211, 212, 213, 213, 214, 215, 216, 216, 217, 218,
        219, 220, 220, 221, 222, 223, 224, 224, 225, 226, 227, 227, 228, 229, 230, 231,
        231, 232, 233, 234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243,
        244, 244, 245, 246, 247, 247, 248, 249, 250, 250, 251, 252, 253, 253, 254, 255 };
      static const char *const *font_data[] = {
        cimg::data_font_small,
        cimg::data_font_normal,
        cimg::data_font_large,
        cimg::data_font_huge };
      static const unsigned int
        font_width[] = { 10,26,52,104 },
        font_height[] = { 13,32,64,128 },
        font_M[] = { 86,91,91,47 },
        font_chunk[] = { sizeof(cimg::data_font_small)/sizeof(char*),
                         sizeof(cimg::data_font_normal)/sizeof(char*),
                         sizeof(cimg::data_font_large)/sizeof(char*),
                         sizeof(cimg::data_font_huge)/sizeof(char*) };
      static const unsigned char font_is_binary[] = { 1,0,0,1 };
      static CImg<ucharT> font_base[4];

      unsigned int ind =
        requested_height<=font_height[0]?0U:
        requested_height<=font_height[1]?1U:
        requested_height<=font_height[2]?2U:3U;

      // Decompress nearest base font data if needed.
      CImg<ucharT> &basef = font_base[ind];
      if (!basef) {
        basef.assign(256*font_width[ind],font_height[ind]);

        unsigned char *ptrd = basef;
        const unsigned char *const ptrde = basef.end();

        // Recompose font data from several chunks, to deal with MS compiler limit with big strings (64 Kb).
        CImg<char> dataf;
        for (unsigned int k = 0; k<font_chunk[ind]; ++k)
          dataf.append(CImg<char>::string(font_data[ind][k],k==font_chunk[ind] - 1,true),'x');

        // Uncompress font data (decode RLE).
        const unsigned int M = font_M[ind];
        if (font_is_binary[ind])
          for (const char *ptrs = dataf; *ptrs; ++ptrs) {
            const int _n = (int)(*ptrs - M - 32), v = _n>=0?255:0, n = _n>=0?_n:-_n;
            if (ptrd + n<=ptrde) { std::memset(ptrd,v,n); ptrd+=n; }
            else { std::memset(ptrd,v,ptrde - ptrd); break; }
          }
        else
          for (const char *ptrs = dataf; *ptrs; ++ptrs) {
            int n = (int)*ptrs - M - 32, v = 0;
            if (n>=0) { v = 85*n; n = 1; }
            else {
              n = -n;
              v = (int)*(++ptrs) - M - 32;
              if (v<0) { v = 0; --ptrs; } else v*=85;
            }
            if (ptrd + n<=ptrde) { std::memset(ptrd,v,n); ptrd+=n; }
            else { std::memset(ptrd,v,ptrde - ptrd); break; }
          }
      }

      // Find optimal font cache location to return.
      static CImgList<ucharT> fonts[16];
      static bool is_variable_widths[16] = { 0 };
      ind = ~0U;
      for (int i = 0; i<16; ++i)
        if (!fonts[i] || (is_variable_widths[i]==is_variable_width && requested_height==fonts[i][0]._height)) {
          ind = (unsigned int)i; break; // Found empty slot or cached font
        }
      if (ind==~0U) { // No empty slots nor existing font in cache
        fonts->assign();
        std::memmove(fonts,fonts + 1,15*sizeof(CImgList<ucharT>));
        std::memmove(is_variable_widths,is_variable_widths + 1,15*sizeof(bool));
        std::memset((void*)(fonts + (ind=15)),0,sizeof(CImgList<ucharT>)); // Free a slot in cache for new font
      }
      CImgList<ucharT> &font = fonts[ind];

      // Render requested font.
      if (!font) {
        const unsigned int padding_x = requested_height<=64?1U:requested_height<=128?2U:3U;
        is_variable_widths[ind] = is_variable_width;
        font = basef.get_split('x',256);
        if (requested_height!=font[0]._height)
          cimglist_for(font,l) {
            font[l].resize(std::max(1U,font[l]._width*requested_height/font[l]._height),requested_height,-100,-100,
                           font[0]._height>requested_height?2:5);
            cimg_for(font[l],ptr,ucharT) *ptr = font_resizemap[*ptr];
          }
        if (is_variable_width) { // Crop font
          cimglist_for(font,l) {
            CImg<ucharT>& letter = font[l];
            int xmin = letter.width(), xmax = 0;
            cimg_forXY(letter,x,y) if (letter(x,y)) { if (x<xmin) xmin = x; if (x>xmax) xmax = x; }
            if (xmin<=xmax) letter.crop(xmin,0,xmax,letter._height - 1);
          }
          font[(int)' '].resize(font[(int)'f']._width,-100,-100,-100,0);
          if (' ' + 256<font.size()) font[' ' + 256].resize(font[(int)'f']._width,-100,-100,-100,0);
          cimglist_for(font,l)
            font[l].resize(std::max(font[(int)';']._width,font[l]._width) + padding_x,
                           -100,1,1,0,0,0.55f);
        } else cimglist_for(font,l) font[l].resize(font[l]._width + padding_x,-100,1,1,0,0,0.55f);
        font.insert(256,0);
        cimglist_for_in(font,0,255,l) font[l].assign(font[l + 256]._width,font[l + 256]._height,1,3,1);