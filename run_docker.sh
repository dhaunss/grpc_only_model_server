docker run  --rm -p 8500:8500 \
  --mount type=bind,source=/home/dhaunss/workdir/grpc-only-cpp-client-/models/best_model,target=/models/best_model \
  -e MODEL_NAME=best_model -t tensorflow/serving 