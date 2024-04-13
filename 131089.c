int Curl_removeHandleFromPipeline(struct SessionHandle *handle,
                                  struct curl_llist *pipeline)
{
  struct curl_llist_element *curr;

  curr = pipeline->head;
  while(curr) {
    if(curr->ptr == handle) {
      Curl_llist_remove(pipeline, curr, NULL);
      return 1; /* we removed a handle */
    }
    curr = curr->next;
  }

  return 0;
}