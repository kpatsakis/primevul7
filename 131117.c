static int call_disconnect_if_dead(struct connectdata *conn,
                                      void *param)
{
  struct SessionHandle* data = (struct SessionHandle*)param;
  disconnect_if_dead(conn, data);
  return 0; /* continue iteration */
}