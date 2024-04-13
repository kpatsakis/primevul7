static Status ConvertShapeToConstant(const string& op, const DataType& type,
                                     const PartialTensorShape& shp,
                                     Tensor* tensor) {
  if (op == "Shape" || op == "ShapeN") {
    *tensor = Tensor(type, TensorShape({shp.dims()}));
    for (int i = 0; i < shp.dims(); ++i) {
      TF_RETURN_IF_ERROR(PutValueIntoTensor(shp.dim_size(i), type, i, tensor));
    }
  } else if (op == "Size") {
    int64_t size = 1;
    for (int i = 0; i < shp.dims(); ++i) {
      size *= shp.dim_size(i);
    }
    *tensor = Tensor(type, TensorShape({}));
    TF_RETURN_IF_ERROR(PutValueIntoTensor(size, type, 0, tensor));
  } else {
    CHECK_EQ(op, "Rank");
    *tensor = Tensor(type, TensorShape({}));
    TF_RETURN_IF_ERROR(PutValueIntoTensor(shp.dims(), type, 0, tensor));
  }
  return Status::OK();
}