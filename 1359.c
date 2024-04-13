Status CreateConstantTensorAttrValue(DataType type, double value,
                                     const TensorShapeProto& shape,
                                     AttrValue* attr_tensor) {
  TensorProto* t = attr_tensor->mutable_tensor();
  t->set_dtype(type);
  *t->mutable_tensor_shape() = shape;
  switch (type) {
    case DT_HALF:
      t->add_half_val(
          Eigen::numext::bit_cast<uint16>(static_cast<Eigen::half>(value)));
      break;
    case DT_BFLOAT16:
      t->add_half_val(
          Eigen::numext::bit_cast<uint16>(static_cast<bfloat16>(value)));
      break;
      SET_TENSOR_VAL_CASE(DT_FLOAT, float, float);
      SET_TENSOR_VAL_CASE(DT_DOUBLE, double, double);
      SET_TENSOR_VAL_CASE(DT_INT64, int64_t, int64);
      SET_TENSOR_VAL_CASE(DT_UINT64, int64_t, int64);
      SET_TENSOR_VAL_CASE(DT_INT32, int32, int);
      SET_TENSOR_VAL_CASE(DT_UINT32, int32, int);
      SET_TENSOR_VAL_CASE(DT_INT16, int32, int);
      SET_TENSOR_VAL_CASE(DT_UINT16, int32, int);
      SET_TENSOR_VAL_CASE(DT_INT8, int32, int);
      SET_TENSOR_VAL_CASE(DT_UINT8, int32, int);
      SET_TENSOR_VAL_CASE(DT_QINT32, int32, int);
      SET_TENSOR_VAL_CASE(DT_QINT16, int32, int);
      SET_TENSOR_VAL_CASE(DT_QUINT16, int32, int);
      SET_TENSOR_VAL_CASE(DT_QINT8, int32, int);
      SET_TENSOR_VAL_CASE(DT_QUINT8, int32, int);
      SET_TENSOR_VAL_CASE(DT_BOOL, bool, bool);
    default:
      return errors::InvalidArgument(
          "Unsupported type in CreateConstantTensorAttrValue: ",
          DataTypeString(type));
  }
  return Status::OK();
}