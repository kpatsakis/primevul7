reAnchorWord(Buffer *buf, Line *l, int spos, int epos)
{
    reAnchorPos(buf, l, &l->lineBuf[spos], &l->lineBuf[epos], _put_anchor_all);
}