static void cleanup_quattro(x3f_quattro_t **QP)
{
  x3f_quattro_t *Q = *QP;

  if (Q == NULL)
    return;

  FREE(Q->top16.buf);
  FREE(Q);

  *QP = NULL;
}