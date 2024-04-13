static struct SessionHandle* gethandleathead(struct curl_llist *pipeline)
{
  struct curl_llist_element *curr = pipeline->head;
  if(curr) {
    return (struct SessionHandle *) curr->ptr;
  }

  return NULL;
}