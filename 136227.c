static void OneLine4(struct ico_progressive_state *context)
{
	gint X;
	guchar *Pixels;

	X = 0;
	if (context->Header.Negative == 0)
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  (context->Header.height - context->Lines - 1));
	else
		Pixels = (context->pixbuf->pixels +
			  (gsize) context->pixbuf->rowstride *
			  context->Lines);
	
	while (X < context->Header.width) {
		guchar Pix;
		
		Pix = context->LineBuf[X/2];

		Pixels[X * 4 + 0] =
		    context->HeaderBuf[4 * (Pix>>4) + 42+context->DIBoffset];
		Pixels[X * 4 + 1] =
		    context->HeaderBuf[4 * (Pix>>4) + 41+context->DIBoffset];
		Pixels[X * 4 + 2] =
		    context->HeaderBuf[4 * (Pix>>4) + 40+context->DIBoffset];
		X++;
		if (X<context->Header.width) { 
			/* Handle the other 4 bit pixel only when there is one */
			Pixels[X * 4 + 0] =
			    context->HeaderBuf[4 * (Pix&15) + 42+context->DIBoffset];
			Pixels[X * 4 + 1] =
			    context->HeaderBuf[4 * (Pix&15) + 41+context->DIBoffset];
			Pixels[X * 4 + 2] =
			    context->HeaderBuf[4 * (Pix&15) + 40+context->DIBoffset];
			X++;
		}
	}
	
}