ospf_fifo_push_head (struct ospf_fifo *fifo, struct ospf_packet *op)
{
  op->next = fifo->head;
  
  if (fifo->tail == NULL)
    fifo->tail = op;
  
  fifo->head = op;
  
  fifo->count++;
}