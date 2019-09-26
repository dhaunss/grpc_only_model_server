# gRPC only cpp client 

Using only grpc to communicate with tensorflow_serving container.

inspired by https://stackoverflow.com/questions/55033952/grpc-only-tensorflow-serving-client-in-c


To use this u need to install:
grpcio ,
grpcio_tools 

to Create DockerImage to serve model use:

docker run -d --name serving_base tensorflow/serving


and copy Model into the container by:

docker cp models/<my model> serving_base:/models/<my model>


inside container run:

tensorflow_model_server --port=8500 --rest_api_port=8501   --model_name=best_model --model_base_path=/models/best_model


to run the model server.

To check the variables and signature of your model run:
saved_model_cli show --dir models/best_model/1 --all






