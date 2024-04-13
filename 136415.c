assegment_data_free (as_t *asdata)
{
  XFREE (MTYPE_AS_SEG_DATA, asdata);
}