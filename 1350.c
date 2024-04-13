Status ConstantFolding::CreateNodeDef(const string& name,
                                      const TensorValue& tensor, NodeDef* node,
                                      size_t original_size) {
  node->set_name(name);
  node->set_op("Const");

  AttrValue attr_type;
  attr_type.set_type(tensor->dtype());
  node->mutable_attr()->insert({"dtype", attr_type});

  AttrValue attr_tensor;
  TensorProto* t = attr_tensor.mutable_tensor();
  bool optimized = false;
  size_t encoded_size;
  // Use the packed representation whenever possible to avoid generating large
  // graphdefs. Moreover, avoid repeating the last values if they're equal.
  if (tensor->NumElements() > 4) {
#define POPULATE_TENSOR_PROTO(tensor, t, TYPE, FIELDTYPE)                      \
  {                                                                            \
    const auto* val_ptr = tensor->flat<TYPE>().data();                         \
    auto last = *val_ptr;                                                      \
    int64_t last_index = 0;                                                    \
    for (int64_t i = 0; i < tensor->NumElements(); ++i) {                      \
      TYPE cur = *val_ptr++;                                                   \
      if (PackedValuesNotEqual(cur, last)) {                                   \
        last = cur;                                                            \
        last_index = i;                                                        \
      }                                                                        \
    }                                                                          \
    encoded_size = (last_index + 1) * sizeof(FIELDTYPE);                       \
    if (encoded_size < kint32max) {                                            \
      optimized = true;                                                        \
      t->mutable_##FIELDTYPE##_val()->Reserve(last_index + 1);                 \
      const auto* src_ptr = tensor->flat<TYPE>().data();                       \
      auto* dst_ptr =                                                          \
          t->mutable_##FIELDTYPE##_val()->AddNAlreadyReserved(last_index + 1); \
      std::copy(src_ptr, src_ptr + last_index + 1, dst_ptr);                   \
    }                                                                          \
  }                                                                            \
  break

    switch (tensor->dtype()) {
      case DT_FLOAT:
        POPULATE_TENSOR_PROTO(tensor, t, float, float);
      case DT_DOUBLE:
        POPULATE_TENSOR_PROTO(tensor, t, double, double);
      case DT_INT64:
        POPULATE_TENSOR_PROTO(tensor, t, int64_t, int64);
      case DT_UINT64:
        POPULATE_TENSOR_PROTO(tensor, t, uint64, uint64);
      case DT_INT32:
        POPULATE_TENSOR_PROTO(tensor, t, int32_t, int);
      case DT_UINT32:
        POPULATE_TENSOR_PROTO(tensor, t, uint32, uint32);
      case DT_INT16:
        POPULATE_TENSOR_PROTO(tensor, t, int16_t, int);
      case DT_UINT16:
        POPULATE_TENSOR_PROTO(tensor, t, uint16, int);
      case DT_INT8:
        POPULATE_TENSOR_PROTO(tensor, t, int8_t, int);
      case DT_UINT8:
        POPULATE_TENSOR_PROTO(tensor, t, uint8, int);
      case DT_BOOL:
        POPULATE_TENSOR_PROTO(tensor, t, bool, bool);
      default:
        /* Do nothing. */
        break;
    }
  }
  if (optimized) {
    // Also specify type and shape.
    t->set_dtype(tensor->dtype());
    tensor->shape().AsProto(t->mutable_tensor_shape());
  } else {
    // DT_HALF, DT_BFLOAT16, DT_QINT32, DT_QINT16, DT_QUINT16, DT_QINT8,
    // DT_QUINT8
    tensor->AsProtoTensorContent(t);
    encoded_size = t->tensor_content().size();
  }
  node->mutable_attr()->insert({"value", attr_tensor});

  if (encoded_size > original_size && encoded_size >= kMaxConstantSize) {
    return errors::InvalidArgument(
        strings::StrCat("Can't fold ", name, ", its size would be too large (",
                        encoded_size, " >= ", kMaxConstantSize, " bytes)"));
  }
  return Status::OK();
}