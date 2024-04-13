test_bson_utf8_key (void)
{
/* euro currency symbol */
#define EU "\xe2\x82\xac"
#define FIVE_EUROS EU EU EU EU EU
   uint32_t length;
   bson_iter_t iter;
   const char *str;
   bson_t *b;
   size_t offset;

   b = get_bson ("eurokey.bson");
   BSON_ASSERT (bson_validate (b, BSON_VALIDATE_NONE, &offset));
   BSON_ASSERT (bson_iter_init (&iter, b));
   BSON_ASSERT (bson_iter_next (&iter));
   BSON_ASSERT (!strcmp (bson_iter_key (&iter), FIVE_EUROS));
   BSON_ASSERT ((str = bson_iter_utf8 (&iter, &length)));
   BSON_ASSERT (length == 15); /* 5 3-byte sequences. */
   BSON_ASSERT (!strcmp (str, FIVE_EUROS));
   bson_destroy (b);
}