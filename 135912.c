png_permit_mng_features (png_structrp png_ptr, png_uint_32 mng_features)
{
   png_debug(1, "in png_permit_mng_features");

   if (png_ptr == NULL)
      return 0;

   png_ptr->mng_features_permitted = mng_features & PNG_ALL_MNG_FEATURES;

   return png_ptr->mng_features_permitted;
}