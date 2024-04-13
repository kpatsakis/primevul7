mj_put_params(gx_device *pdev,  gs_param_list *plist, int ptype)
{
        int old_bpp = mj->color_info.depth;
        int bpp = 0;
        int code = 0;
        int density = mj->density;
        int cyan = mj->cyan;
        int magenta = mj->magenta;
        int yellow = mj->yellow;
        int black = mj->black;
        bool direction = mj->direction;
        bool microweave = mj->microweave;
        int dotsize = mj->dotsize;
        gs_param_name param_name;
        int ecode = 0;

        code = put_param_int(plist, "Density", &density, 0, INT_MAX, code);
        code = put_param_int(plist, "Cyan", &cyan, 0, INT_MAX, code);
        code = put_param_int(plist, "Magenta", &magenta, 0, INT_MAX, code);
        code = put_param_int(plist, "Yellow", &yellow, 0, INT_MAX, code);
        code = put_param_int(plist, "Black", &black, 0, INT_MAX, code);

        if ((code = param_read_bool(plist,
                                     (param_name = "Unidirectional"),
                                     &direction))< 0) {
          param_signal_error(plist, param_name, ecode = code);
        }

        if ((code = param_read_bool(plist,
                                     (param_name = "Microweave"),
                                     &microweave))< 0) {
          param_signal_error(plist, param_name, ecode = code);
        }
        if (ecode < 0)
          return code;

        if (microweave && pdev->x_pixels_per_inch == 180)
            return_error(gs_error_rangecheck);

        code = put_param_int(plist, "DotSize", &dotsize, 0, 1, code);
        code = put_param_int(plist, "BitsPerPixel", &bpp, 1, 32, code);

        if ( code < 0 )
          return code;

        mj->density = density;
        mj->cyan = cyan;
        mj->magenta = magenta;
        mj->yellow = yellow;
        mj->black = black;
        mj->direction = direction;
        mj->microweave = microweave;
        mj->dotsize = dotsize;

        if ( bpp != 0 ) {
          if (bpp != 8 && bpp != 32)
            return_error(gs_error_rangecheck);

          if (ptype == MJ500C)
            mj500c_set_bpp(pdev, bpp);
          else
            set_bpp(pdev, bpp);
          gdev_prn_put_params(pdev, plist);
          if ( bpp != old_bpp && pdev->is_open )
            return gs_closedevice(pdev);
          return 0;
        }
        else
          return gdev_prn_put_params(pdev, plist);
}