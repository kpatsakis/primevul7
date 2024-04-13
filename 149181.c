initialize_cclass(CClassNode* cc)
{
  BITSET_CLEAR(cc->bs);
  cc->flags = 0;
  cc->mbuf  = NULL;
}