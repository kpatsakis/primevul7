Status ConstantFolding::MaterializeShapes(const GraphProperties& properties) {
  // We may add some nodes to the graph to encode control dependencies and hold
  // the materialized shapes: there is no need to process these added nodes, so
  // only iterate over the nodes of the input graph.
  const int node_count = graph_->node_size();
  for (int node_idx = 0; node_idx < node_count; ++node_idx) {
    NodeDef* node = graph_->mutable_node(node_idx);
    const string op = node->op();
    if (op != "Shape" && op != "Size" && op != "Rank" && op != "ShapeN" &&
        op != "TensorArraySizeV3") {
      continue;
    }
    const std::vector<OpInfo::TensorProperties>& output =
        properties.GetOutputProperties(node->name());
    const std::vector<OpInfo::TensorProperties>& input =
        properties.GetInputProperties(node->name());
    if (input.empty() || output.empty()) {
      continue;
    }

    if (op == "Shape" || op == "Size" || op == "Rank") {
      CHECK_EQ(1, output.size());
      CHECK_EQ(1, input.size());

      const DataType type = output[0].dtype();
      CHECK(type == DT_INT32 || type == DT_INT64);
      const PartialTensorShape shape(input[0].shape());

      if ((op != "Rank" && !shape.IsFullyDefined()) ||
          (op == "Rank" && shape.unknown_rank())) {
        continue;
      }

      Tensor constant_value(type);
      if (!ConvertShapeToConstant(op, type, shape, &constant_value).ok()) {
        continue;
      }

      // TODO(rmlarsen): Remove this workaround for b/150861569
      // The bug involves an expression of the form Shape(ExpandDims(x)
      // with an incorrectly inferred zero-size first dimension.
      if (op == "Shape") {
        if (shape.dims() > 0 && shape.dim_size(0) == 0) continue;
      }

      // Repurpose the existing node to be the constant.
      // Device placement is preserved.
      graph_modified_ = true;
      node->set_op("Const");
      EraseRegularNodeAttributes(node);
      (*node->mutable_attr())["dtype"].set_type(type);
      constant_value.AsProtoTensorContent(
          (*node->mutable_attr())["value"].mutable_tensor());

      // Turn the data input into a control dependency: this is needed to
      // ensure that the constant value will only be run in the
      // cases where the shape/rank/size would have been run in
      // the original graph.
      string ctrl_dep =
          AddControlDependency(node->input(0), graph_, node_map_.get());
      node_map_->UpdateInput(node->name(), node->input(0), ctrl_dep);
      node->set_input(0, ctrl_dep);
      // Done with the Shape/Size/Rank node, move to the next node.
      continue;
    }

    if (op == "TensorArraySizeV3") {
      const NodeDef* array = CHECK_NOTNULL(node_map_->GetNode(node->input(0)));
      if (array->input_size() == 0 ||
          (array->attr().count("dynamic_size") != 0 &&
           array->attr().at("dynamic_size").b())) {
        continue;
      }
      const NodeDef* array_size =
          CHECK_NOTNULL(node_map_->GetNode(array->input(0)));
      if (IsReallyConstant(*array_size)) {
        // Don't materialize 0 sizes to avoid triggering incorrect static
        // checks. A 0 sized array that can't grow isn't useful anyway.
        if (array_size->attr().count("value") == 0) {
          continue;
        }
        const TensorProto& raw_val = array_size->attr().at("value").tensor();
        if (raw_val.dtype() != DT_INT32) {
          continue;
        }
        Tensor value(raw_val.dtype(), raw_val.tensor_shape());
        if (!value.FromProto(raw_val)) {
          continue;
        }
        if (value.flat<int32>()(0) == 0) {
          continue;
        }

        graph_modified_ = true;
        node->set_op("Const");
        *node->mutable_attr() = array_size->attr();
        node->set_input(0, AsControlDependency(NodeName(node->input(0))));
        node->set_input(1, AddControlDependency(NodeName(node->input(1)),
                                                graph_, node_map_.get()));
      }
      continue;
    }

    // Handle ShapeN materialization case.
    // It's possible that not all input tensors have known shapes.
    CHECK_EQ(op, "ShapeN");
    CHECK_EQ(input.size(), output.size());
    const NodeDef* const shape_n_node = node;
    for (int port_idx = 0, idx_limit = output.size(); port_idx < idx_limit;
         ++port_idx) {
      const DataType type = output[port_idx].dtype();
      CHECK(type == DT_INT32 || type == DT_INT64);
      const PartialTensorShape shape(input[port_idx].shape());
      if (!shape.IsFullyDefined()) {
        continue;
      }
      Tensor constant_value(type);
      auto status = ConvertShapeToConstant(op, type, shape, &constant_value);
      if (!status.ok()) {
        continue;
      }

      // We make a copy because we mutate the nodes.
      auto fanouts = node_map_->GetOutputs(shape_n_node->name());
      // Find all nodes consuming this shape and connect them through the new
      // constant node instead.
      for (NodeDef* output : fanouts) {
        // Track whether there are any direct edges left between shape_n_node
        // and this output node after the transformation.
        bool direct_edges_exist = false;
        for (int k = 0; k < output->input_size(); ++k) {
          int port;
          const string node_name = ParseNodeName(output->input(k), &port);
          if (node_name == shape_n_node->name() && port == port_idx) {
            // Create a const node as ShapeN's output if not already.
            const string const_name = OptimizedNodeName(
                *shape_n_node, strings::StrCat("-matshapes-", port_idx));
            if (node_map_->GetNode(const_name) == nullptr) {
              NodeDef* added_node = graph_->add_node();
              added_node->set_name(const_name);
              added_node->set_op("Const");
              added_node->set_device(shape_n_node->device());
              node_map_->AddNode(added_node->name(), added_node);
              (*added_node->mutable_attr())["dtype"].set_type(type);
              constant_value.AsProtoTensorContent(
                  (*added_node->mutable_attr())["value"].mutable_tensor());
              // We add a control dependency to the original ShapeN node,
              // so that the node will only be run if all inputs of the
              // original ShapeN node are run.
              string ctrl_dep = AddControlDependency(shape_n_node->name(),
                                                     graph_, node_map_.get());
              *added_node->add_input() = ctrl_dep;
              node_map_->AddOutput(NodeName(ctrl_dep), added_node->name());
            }
            *output->mutable_input(k) = const_name;
            node_map_->AddOutput(const_name, output->name());
            graph_modified_ = true;
          }
          if (node_name == shape_n_node->name() && port != port_idx) {
            direct_edges_exist = true;
          }
        }
        if (!direct_edges_exist) {
          node_map_->RemoveOutput(node->name(), output->name());
        }
      }
    }
  }

  return Status::OK();
}