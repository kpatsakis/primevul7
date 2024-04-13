rb_enc_asciicompat(OnigEncoding enc)
{
  return ONIGENC_MBC_MINLEN(enc)==1 && !((enc)->ruby_encoding_index & ENC_DUMMY_FLAG);
}