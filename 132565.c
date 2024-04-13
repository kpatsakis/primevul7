MagickPrivate int XCommandWidget(Display *display,XWindows *windows,
  const char *const *selections,XEvent *event)
{
#define tile_width 112
#define tile_height 70

  static const unsigned char
    tile_bits[]=
    {
      0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x1e, 0x38, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x1e, 0xbc, 0x9f, 0x03, 0x00, 0x3e, 0x00, 0xc0,
      0x1f, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x1e, 0xfc, 0xff, 0x0f, 0x80, 0x3f,
      0x00, 0xf0, 0x1f, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0xfc, 0xff, 0x1f,
      0xe0, 0x3f, 0x00, 0xfc, 0x1f, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0xfc,
      0xff, 0x1f, 0xf0, 0x3f, 0x00, 0xfe, 0x1f, 0xf8, 0x0f, 0x00, 0x00, 0x00,
      0x1e, 0xfc, 0xfc, 0x3f, 0xf8, 0x3f, 0x00, 0xff, 0x1e, 0xfc, 0x0f, 0x00,
      0x00, 0x00, 0x1e, 0x7c, 0xfc, 0x3e, 0xf8, 0x3c, 0x80, 0x1f, 0x1e, 0x7c,
      0x0f, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x78, 0x3c, 0x7c, 0x3c, 0xc0, 0x0f,
      0x1e, 0x3e, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x78, 0x3c, 0x7c, 0x3c,
      0xc0, 0x07, 0x1e, 0x3e, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x78, 0x3c,
      0x7c, 0x7c, 0xc0, 0x0f, 0x1e, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x78,
      0x78, 0x3c, 0xfc, 0x7c, 0x80, 0x7f, 0x1e, 0x7c, 0x00, 0x00, 0x00, 0x00,
      0x1e, 0xf8, 0x78, 0x7c, 0xf8, 0xff, 0x00, 0xff, 0x1f, 0xf8, 0xff, 0x00,
      0x00, 0x00, 0x1e, 0xf8, 0x78, 0x7c, 0xf0, 0xff, 0x07, 0xfe, 0x1f, 0xf8,
      0xff, 0x00, 0x00, 0x00, 0x1e, 0xf8, 0x78, 0x7c, 0xf0, 0xff, 0x07, 0xf8,
      0x1f, 0xf0, 0xff, 0x01, 0x00, 0x00, 0x1e, 0xf8, 0x78, 0x7c, 0xc0, 0xef,
      0x07, 0xe0, 0x1f, 0xc0, 0xff, 0x01, 0x00, 0x00, 0x1e, 0x70, 0x40, 0x78,
      0x00, 0xc7, 0x07, 0x00, 0x1e, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x1e, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
      0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x02, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x07,
      0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0xc0, 0x0f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x60, 0x00, 0xc0, 0x0f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x78, 0x00, 0xc0, 0x8f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0xc0, 0x8f, 0x3f, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0xe0, 0x9f, 0x7f, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0xe0, 0xdf,
      0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x78, 0x00,
      0xe0, 0xdf, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x0c,
      0x78, 0x30, 0xf0, 0xff, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e,
      0x00, 0x0f, 0xf8, 0x70, 0xf0, 0xff, 0x7b, 0x00, 0x00, 0x1f, 0x00, 0xe0,
      0x0f, 0x1e, 0x80, 0x0f, 0xf8, 0x78, 0xf0, 0xfd, 0xf9, 0x00, 0xc0, 0x1f,
      0x00, 0xf8, 0x0f, 0x00, 0xe0, 0x1f, 0xf8, 0x7c, 0xf0, 0xfc, 0xf9, 0x00,
      0xf0, 0x1f, 0x00, 0xfe, 0x0f, 0x00, 0xf0, 0x07, 0xf8, 0x3e, 0xf8, 0xfc,
      0xf0, 0x01, 0xf8, 0x1f, 0x00, 0xff, 0x0f, 0x1e, 0xf0, 0x03, 0xf8, 0x3f,
      0xf8, 0xf8, 0xf0, 0x01, 0xfc, 0x1f, 0x80, 0x7f, 0x0f, 0x1e, 0xf8, 0x00,
      0xf8, 0x1f, 0x78, 0x18, 0xf0, 0x01, 0x7c, 0x1e, 0xc0, 0x0f, 0x0f, 0x1e,
      0x7c, 0x00, 0xf0, 0x0f, 0x78, 0x00, 0xf0, 0x01, 0x3e, 0x1e, 0xe0, 0x07,
      0x0f, 0x1e, 0x7c, 0x00, 0xf0, 0x07, 0x7c, 0x00, 0xe0, 0x01, 0x3e, 0x1e,
      0xe0, 0x03, 0x0f, 0x1e, 0x3e, 0x00, 0xf0, 0x0f, 0x7c, 0x00, 0xe0, 0x03,
      0x3e, 0x3e, 0xe0, 0x07, 0x0f, 0x1e, 0x1e, 0x00, 0xf0, 0x1f, 0x3c, 0x00,
      0xe0, 0x03, 0x7e, 0x3e, 0xc0, 0x3f, 0x0f, 0x1e, 0x3e, 0x00, 0xf0, 0x1f,
      0x3e, 0x00, 0xe0, 0x03, 0xfc, 0x7f, 0x80, 0xff, 0x0f, 0x1e, 0xfc, 0x00,
      0xf0, 0x3e, 0x3e, 0x00, 0xc0, 0x03, 0xf8, 0xff, 0x03, 0xff, 0x0f, 0x1e,
      0xfc, 0x07, 0xf0, 0x7c, 0x1e, 0x00, 0xc0, 0x03, 0xf8, 0xff, 0x03, 0xfc,
      0x0f, 0x1e, 0xf8, 0x1f, 0xf0, 0xf8, 0x1e, 0x00, 0xc0, 0x03, 0xe0, 0xf7,
      0x03, 0xf0, 0x0f, 0x1e, 0xe0, 0x3f, 0xf0, 0x78, 0x1c, 0x00, 0x80, 0x03,
      0x80, 0xe3, 0x03, 0x00, 0x0f, 0x1e, 0xc0, 0x3f, 0xf0, 0x30, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0e, 0x00, 0x3e, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00, 0x10,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
      0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

  int
    id,
    y;

  int
    i;

  static unsigned int
    number_selections;

  unsigned int
    height;

  size_t
    state;

  XFontStruct
    *font_info;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  font_info=windows->command.font_info;
  height=(unsigned int) (font_info->ascent+font_info->descent);
  id=(~0);
  state=DefaultState;
  if (event == (XEvent *) NULL)
    {
      unsigned int
        width;

      XTextProperty
        window_name;

      XWindowChanges
        window_changes;

      /*
        Determine command window attributes.
      */
      assert(selections != (const char **) NULL);
      windows->command.width=0;
      for (i=0; selections[i] != (char *) NULL; i++)
      {
        width=WidgetTextWidth(font_info,(char *) selections[i]);
        if (width > windows->command.width)
          windows->command.width=width;
      }
      number_selections=(unsigned int) i;
      windows->command.width+=3*QuantumMargin+10;
      if ((int) windows->command.width < (tile_width+QuantumMargin+10))
        windows->command.width=(unsigned  int) (tile_width+QuantumMargin+10);
      windows->command.height=(unsigned  int) (number_selections*
        (((3*height) >> 1)+10)+tile_height+20);
      windows->command.min_width=windows->command.width;
      windows->command.min_height=windows->command.height;
      XConstrainWindowPosition(display,&windows->command);
      if (windows->command.id != (Window) NULL)
        {
          Status
            status;

          /*
            Reconfigure command window.
          */
          status=XStringListToTextProperty(&windows->command.name,1,
            &window_name);
          if (status != False)
            {
              XSetWMName(display,windows->command.id,&window_name);
              XSetWMIconName(display,windows->command.id,&window_name);
              (void) XFree((void *) window_name.value);
            }
          window_changes.width=(int) windows->command.width;
          window_changes.height=(int) windows->command.height;
          (void) XReconfigureWMWindow(display,windows->command.id,
            windows->command.screen,(unsigned int) (CWWidth | CWHeight),
            &window_changes);
        }
      /*
        Allocate selection info memory.
      */
      if (selection_info != (XWidgetInfo *) NULL)
        selection_info=(XWidgetInfo *) RelinquishMagickMemory(selection_info);
      selection_info=(XWidgetInfo *) AcquireQuantumMemory(number_selections,
        sizeof(*selection_info));
      if (selection_info == (XWidgetInfo *) NULL)
        {
          ThrowXWindowFatalException(ResourceLimitFatalError,
            "MemoryAllocationFailed","...");
          return(id);
        }
      state|=UpdateConfigurationState | RedrawWidgetState;
    }
  /*
    Wait for next event.
  */
  if (event != (XEvent *) NULL)
    switch (event->type)
    {
      case ButtonPress:
      {
        for (i=0; i < (int) number_selections; i++)
        {
          if (MatteIsActive(selection_info[i],event->xbutton) == MagickFalse)
            continue;
          if (i >= (int) windows->command.data)
            {
              selection_info[i].raised=MagickFalse;
              XDrawBeveledButton(display,&windows->command,&selection_info[i]);
              break;
            }
          submenu_info=selection_info[i];
          submenu_info.active=MagickTrue;
          toggle_info.y=submenu_info.y+(submenu_info.height >> 1)-
            (toggle_info.height >> 1);
          id=i;
          (void) XCheckWindowEvent(display,windows->widget.id,LeaveWindowMask,
            event);
          break;
        }
        break;
      }
      case ButtonRelease:
      {
        for (i=0; i < (int) number_selections; i++)
        {
          if (MatteIsActive(selection_info[i],event->xbutton) == MagickFalse)
            continue;
          id=i;
          if (id >= (int) windows->command.data)
            {
              selection_info[id].raised=MagickTrue;
              XDrawBeveledButton(display,&windows->command,&selection_info[id]);
              break;
            }
          break;
        }
        break;
      }
      case ClientMessage:
      {
        /*
          If client window delete message, withdraw command widget.
        */
        if (event->xclient.message_type != windows->wm_protocols)
          break;
        if (*event->xclient.data.l != (int) windows->wm_delete_window)
          break;
        (void) XWithdrawWindow(display,windows->command.id,
          windows->command.screen);
        break;
      }
      case ConfigureNotify:
      {
        /*
          Update widget configuration.
        */
        if (event->xconfigure.window != windows->command.id)
          break;
        if (event->xconfigure.send_event != 0)
          {
            windows->command.x=event->xconfigure.x;
            windows->command.y=event->xconfigure.y;
          }
        if ((event->xconfigure.width == (int) windows->command.width) &&
            (event->xconfigure.height == (int) windows->command.height))
          break;
        windows->command.width=(unsigned int)
          MagickMax(event->xconfigure.width,(int) windows->command.min_width);
        windows->command.height=(unsigned int)
          MagickMax(event->xconfigure.height,(int) windows->command.min_height);
        state|=UpdateConfigurationState;
        break;
      }
      case Expose:
      {
        if (event->xexpose.window != windows->command.id)
          break;
        if (event->xexpose.count != 0)
          break;
        state|=RedrawWidgetState;
        break;
      }
      case MotionNotify:
      {
        /*
          Return the ID of the highlighted menu entry.
        */
        for ( ; ; )
        {
          for (i=0; i < (int) number_selections; i++)
          {
            if (i >= (int) windows->command.data)
              {
                if (selection_info[i].raised ==
                    MatteIsActive(selection_info[i],event->xmotion))
                  {
                    /*
                      Button status changed.
                    */
                    selection_info[i].raised=!selection_info[i].raised;
                    XDrawBeveledButton(display,&windows->command,
                      &selection_info[i]);
                  }
                continue;
              }
            if (MatteIsActive(selection_info[i],event->xmotion) == MagickFalse)
              continue;
            submenu_info=selection_info[i];
            submenu_info.active=MagickTrue;
            toggle_info.raised=MagickTrue;
            toggle_info.y=submenu_info.y+(submenu_info.height >> 1)-
              (toggle_info.height >> 1);
            XDrawTriangleEast(display,&windows->command,&toggle_info);
            id=i;
          }
          XDelay(display,SuspendTime);
          if (XCheckMaskEvent(display,ButtonMotionMask,event) == MagickFalse)
            break;
          while (XCheckMaskEvent(display,ButtonMotionMask,event)) ;
          toggle_info.raised=MagickFalse;
          if (windows->command.data != 0)
            XDrawTriangleEast(display,&windows->command,&toggle_info);
        }
        break;
      }
      case MapNotify:
      {
        windows->command.mapped=MagickTrue;
        break;
      }
      case UnmapNotify:
      {
        windows->command.mapped=MagickFalse;
        break;
      }
      default:
        break;
    }
  if (state & UpdateConfigurationState)
    {
      /*
        Initialize button information.
      */
      assert(selections != (const char **) NULL);
      y=tile_height+20;
      for (i=0; i < (int) number_selections; i++)
      {
        XGetWidgetInfo(selections[i],&selection_info[i]);
        selection_info[i].center=MagickFalse;
        selection_info[i].bevel_width--;
        selection_info[i].height=(unsigned int) ((3*height) >> 1);
        selection_info[i].x=(QuantumMargin >> 1)+4;
        selection_info[i].width=(unsigned int) (windows->command.width-
          (selection_info[i].x << 1));
        selection_info[i].y=y;
        y+=selection_info[i].height+(selection_info[i].bevel_width << 1)+6;
      }
      XGetWidgetInfo((char *) NULL,&toggle_info);
      toggle_info.bevel_width--;
      toggle_info.width=(unsigned int) (((5*height) >> 3)-
        (toggle_info.bevel_width << 1));
      toggle_info.height=toggle_info.width;
      toggle_info.x=selection_info[0].x+selection_info[0].width-
        toggle_info.width-(QuantumMargin >> 1);
      if (windows->command.mapped)
        (void) XClearWindow(display,windows->command.id);
    }
  if (state & RedrawWidgetState)
    {
      Pixmap
        tile_pixmap;

      /*
        Draw command buttons.
      */
      tile_pixmap=XCreatePixmapFromBitmapData(display,windows->command.id,
        (char *) tile_bits,tile_width,tile_height,1L,0L,1);
      if (tile_pixmap != (Pixmap) NULL)
        {
          (void) XCopyPlane(display,tile_pixmap,windows->command.id,
            windows->command.annotate_context,0,0,tile_width,tile_height,
            (int) ((windows->command.width-tile_width) >> 1),10,1L);
          (void) XFreePixmap(display,tile_pixmap);
        }
      for (i=0; i < (int) number_selections; i++)
      {
        XDrawBeveledButton(display,&windows->command,&selection_info[i]);
        if (i >= (int) windows->command.data)
          continue;
        toggle_info.raised=MagickFalse;
        toggle_info.y=selection_info[i].y+(selection_info[i].height >> 1)-
          (toggle_info.height >> 1);
        XDrawTriangleEast(display,&windows->command,&toggle_info);
      }
      XHighlightWidget(display,&windows->command,BorderOffset,BorderOffset);
    }
  return(id);
}