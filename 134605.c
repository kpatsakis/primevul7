int LibRaw::open_file(const wchar_t *fname, INT64 max_buf_size)
{
  struct _stati64 st;
  if(_wstati64(fname,&st))	
    return LIBRAW_IO_ERROR;
  int big = (st.st_size > max_buf_size)?1:0;

  LibRaw_abstract_datastream *stream;
  try {
    if(big)
      stream = new LibRaw_bigfile_datastream(fname);
    else
      stream = new LibRaw_file_datastream(fname);
  }

  catch (std::bad_alloc)
    {
      recycle();
      return LIBRAW_UNSUFFICIENT_MEMORY;
    }
  if(!stream->valid())
    {
      delete stream;
      return LIBRAW_IO_ERROR;
    }
  ID.input_internal = 0; // preserve from deletion on error
  int ret = open_datastream(stream);
  if (ret == LIBRAW_SUCCESS)
    {
      ID.input_internal =1 ; // flag to delete datastream on recycle
    }
  else
    {
      delete stream;
      ID.input_internal = 0;
    }
  return ret;
}