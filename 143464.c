    //! Wait for any event occuring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp6.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event) wait_all();