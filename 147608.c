bson_iter_document (const bson_iter_t *iter,  /* IN */
                    uint32_t *document_len,   /* OUT */
                    const uint8_t **document) /* OUT */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (document_len);
   BSON_ASSERT (document);

   *document = NULL;
   *document_len = 0;

   if (ITER_TYPE (iter) == BSON_TYPE_DOCUMENT) {
      memcpy (document_len, (iter->raw + iter->d1), sizeof (*document_len));
      *document_len = BSON_UINT32_FROM_LE (*document_len);
      *document = (iter->raw + iter->d1);
   }
}