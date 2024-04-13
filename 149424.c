gst_h264_write_sei_registered_user_data (NalWriter * nw,
    GstH264RegisteredUserData * rud)
{
  WRITE_UINT8 (nw, rud->country_code, 8);
  if (rud->country_code == 0xff)
    WRITE_UINT8 (nw, rud->country_code_extension, 8);

  WRITE_BYTES (nw, rud->data, rud->size);

  return TRUE;

error:
  return FALSE;
}