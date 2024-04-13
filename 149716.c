set_sub_anchor(regex_t* reg, OptAncInfo* anc)
{
  reg->sub_anchor |= anc->left_anchor  & ANCHOR_BEGIN_LINE;
  reg->sub_anchor |= anc->right_anchor & ANCHOR_END_LINE;
}