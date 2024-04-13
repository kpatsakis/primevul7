static Status PutValueIntoTensor(const int64_t value, const DataType& type,
                                 const int index, Tensor* tensor) {
  if (type == DT_INT32) {
    if (value >= INT_MAX) {
      return Status(error::INVALID_ARGUMENT, "int32 overflow");
    }
    tensor->flat<int32>()(index) = static_cast<int32>(value);
  } else {
    tensor->flat<int64_t>()(index) = value;
  }
  return Status::OK();
}