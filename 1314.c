static void XEditText(Display *display,XWidgetInfo *text_info,
  const KeySym key_symbol,char *text,const size_t state)
{
  switch ((int) key_symbol)
  {
    case XK_BackSpace:
    case XK_Delete:
    {
      if (text_info->highlight)
        {
          /*
            Erase the entire line of text.
          */
          *text_info->text='\0';
          text_info->cursor=text_info->text;
          text_info->marker=text_info->text;
          text_info->highlight=MagickFalse;
        }
      /*
        Erase one character.
      */
      if (text_info->cursor != text_info->text)
        {
          text_info->cursor--;
          (void) memmove(text_info->cursor,text_info->cursor+1,
            strlen(text_info->cursor+1)+1);
          text_info->highlight=MagickFalse;
          break;
        }
    }
    case XK_Left:
    case XK_KP_Left:
    {
      /*
        Move cursor one position left.
      */
      if (text_info->cursor == text_info->text)
        break;
      text_info->cursor--;
      break;
    }
    case XK_Right:
    case XK_KP_Right:
    {
      /*
        Move cursor one position right.
      */
      if (text_info->cursor == (text_info->text+Extent(text_info->text)))
        break;
      text_info->cursor++;
      break;
    }
    default:
    {
      char
        *p,
        *q;

      int
        i;

      if (state & ControlState)
        break;
      if (*text == '\0')
        break;
      if ((Extent(text_info->text)+1) >= (int) MaxTextExtent)
        (void) XBell(display,0);
      else
        {
          if (text_info->highlight)
            {
              /*
                Erase the entire line of text.
              */
              *text_info->text='\0';
              text_info->cursor=text_info->text;
              text_info->marker=text_info->text;
              text_info->highlight=MagickFalse;
            }
          /*
            Insert a string into the text.
          */
          q=text_info->text+Extent(text_info->text)+strlen(text);
          for (i=0; i <= Extent(text_info->cursor); i++)
          {
            *q=(*(q-Extent(text)));
            q--;
          }
          p=text;
          for (i=0; i < Extent(text); i++)
            *text_info->cursor++=(*p++);
        }
      break;
    }
  }
}