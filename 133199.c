static x3f_quattro_t *new_quattro(x3f_quattro_t **QP)
{
  x3f_quattro_t *Q = (x3f_quattro_t *)calloc(1, sizeof(x3f_quattro_t));
  int i;

  cleanup_quattro(QP);

  for (i = 0; i < TRUE_PLANES; i++)
  {
    Q->plane[i].columns = 0;
    Q->plane[i].rows = 0;
  }

  Q->unknown = 0;

  Q->top16.data = NULL;
  Q->top16.buf = NULL;

  *QP = Q;

  return Q;
}