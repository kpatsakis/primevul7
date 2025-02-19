void LibRaw::x3f_load_raw()
{
  int raise_error=0;
  x3f_t *x3f = (x3f_t*)_x3f_data;
  if(!x3f) return; // No data pointer set
  if(X3F_OK == x3f_load_data(x3f, x3f_get_raw(x3f)))
    {
      x3f_directory_entry_t *DE = x3f_get_raw(x3f);
      x3f_directory_entry_header_t *DEH = &DE->header;
      x3f_image_data_t *ID = &DEH->data_subsection.image_data;
      x3f_huffman_t *HUF = ID->huffman;
      x3f_true_t *TRU = ID->tru;
      uint16_t *data = NULL;
      if(ID->rows != S.raw_height || ID->columns != S.raw_width)
        {
          raise_error = 1;
          goto end;
        }
      if (HUF != NULL)
        data = HUF->x3rgb16.element;
      if (TRU != NULL)
        data = TRU->x3rgb16.element;
      if (data == NULL) 
        {
          raise_error = 1;
          goto end;
        }
      imgdata.rawdata.color3_image = (ushort (*)[3])data;

	  if(!strcasecmp(imgdata.idata.make,"Sigma") 
		  && (!strcasecmp(imgdata.idata.model,"dp2 Quattro")  || !strcasecmp(imgdata.idata.model,"dp1 Quattro"))
		  && (imgdata.params.x3f_flags & LIBRAW_DP2Q_INTERPOLATEAF)
		  )
	  {
		  if(imgdata.sizes.raw_width == 5888)
		  {
			  x3f_dpq_interpolate_af(32,8,2);
		  }
		  if(imgdata.sizes.raw_width == 2944)
		  {
			  x3f_dpq_interpolate_af(16,4,1);
		  }
	  }

	  if(!strcasecmp(imgdata.idata.make,"Sigma") 
		  && (!strcasecmp(imgdata.idata.model,"dp2 Quattro") || !strcasecmp(imgdata.idata.model,"dp1 Quattro"))
		  && (imgdata.params.x3f_flags & LIBRAW_DP2Q_INTERPOLATERG) 
		  && (imgdata.sizes.raw_width== 5888)
		  )
			x3f_dpq_interpolate_rg();

  
  }
  else
    raise_error = 1;
end:
  if(raise_error)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
}