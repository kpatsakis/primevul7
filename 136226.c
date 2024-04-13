static void OneLineTransp(struct ico_progressive_state *context)
{
	gint X;
	guchar *Pixels;

	/* Ignore the XOR mask for XP style 32-bpp icons with alpha */ 
	if (context->Header.depth == 32)
		return;

	X = 0;
	if (context->Header.Negative == 0)
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  (2*context->Header.height - context->Lines - 1));
	else
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  (context->Lines-context->Header.height));
	while (X < context->Header.width) {
		int Bit;

		Bit = (context->LineBuf[X / 8]) >> (7 - (X & 7));
		Bit = Bit & 1;
		/* The joys of having a BGR byteorder */
		Pixels[X * 4 + 3] = 255-Bit*255;
#if 0
		if (Bit){
		  Pixels[X*4+0] = 255;
		  Pixels[X*4+1] = 255;
		} else {
		  Pixels[X*4+0] = 0;
		  Pixels[X*4+1] = 0;
		}
#endif		
		X++;
	}
}