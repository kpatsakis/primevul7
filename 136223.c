OneLine16 (struct ico_progressive_state *context)
{
        int i;
        guchar *pixels;
        guchar *src;

        if (context->Header.Negative == 0)
                pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
                          (context->Header.height - context->Lines - 1));
        else
                pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
                          context->Lines);

        src = context->LineBuf;

        for (i = 0; i < context->Header.width; i++) {
                int v, r, g, b;

                v = (int) src[0] | ((int) src[1] << 8);
                src += 2;

                /* Extract 5-bit RGB values */

                r = (v >> 10) & 0x1f;
                g = (v >> 5) & 0x1f;
                b = v & 0x1f;

                /* Fill the rightmost bits to form 8-bit values */

                *pixels++ = (r << 3) | (r >> 2);
                *pixels++ = (g << 3) | (g >> 2);
                *pixels++ = (b << 3) | (b >> 2);
                pixels++; /* skip alpha channel */
        }
}