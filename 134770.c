test_bson_init_static (void)
{
   static const uint8_t data[5] = {5};
   bson_t b;

   bson_init_static (&b, data, sizeof data);
   BSON_ASSERT ((b.flags & BSON_FLAG_RDONLY));
   bson_destroy (&b);
}