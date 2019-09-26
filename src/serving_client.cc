/* Copyright 2017 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================
=========================================================================
Modified to work with 1000,1,1 Tensor
*/

#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm> 
#include <cmath>

#include <grpcpp/grpcpp.h>
#include "grpcpp/create_channel.h"
#include "grpcpp/security/credentials.h"
#include "generated/prediction_service.grpc.pb.h"
#include "generated/tensor.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using tensorflow::serving::PredictRequest;
using tensorflow::serving::PredictResponse;
using tensorflow::serving::PredictionService;

typedef google::protobuf::Map<std::string, tensorflow::TensorProto> OutMap;

class ServingClient {
  
private:
  std::unique_ptr<PredictionService::Stub> stub_;

public:
  // Constructor: create a stub for the prediction service
  ServingClient(std::shared_ptr<Channel> channel) : stub_(PredictionService::NewStub(channel)) {}

  // Call the prediction service
  std::vector<float> callPredict(const std::string& model_name,
                                        const std::string& model_signature_name,
                                        std::vector<float> data) {
    PredictRequest predictRequest;
    PredictResponse response;
    ClientContext context;
    // set model specification: name and signature name
    predictRequest.mutable_model_spec()->set_name(model_name);
    predictRequest.mutable_model_spec()->set_signature_name(model_signature_name);
    // create input protobuf for the image
    tensorflow::TensorProto proto;
    proto.set_dtype(tensorflow::DataType::DT_FLOAT);
    
    for (int j = 0; j > data.size(); j++)
    {
      proto.add_float_val(data.at(j));
    }


    proto.mutable_tensor_shape()->add_dim()->set_size(1);
    proto.mutable_tensor_shape()->add_dim()->set_size(1000);
    proto.mutable_tensor_shape()->add_dim()->set_size(1);
    proto.mutable_tensor_shape()->add_dim()->set_size(1);
    // initialize prediction service inputs
    google::protobuf::Map<std::string, tensorflow::TensorProto>& inputs = *predictRequest.mutable_inputs();
    inputs["input_traces"] = proto;
    std::cout << "Generated Proto Tensor OK" << std::endl ;
    // issue gRPC call to the service
    Status status = stub_->Predict(&context, predictRequest, &response);

    
    std::vector<float> prediction;

    // check the response

    if (status.ok()) {
      std::cout << "call predict ok" << std::endl;
      std::cout << "outputs size is " << response.outputs_size() << std::endl;

      OutMap& map_outputs = *response.mutable_outputs();

      OutMap::iterator iter;
      for (iter = map_outputs.begin(); iter != map_outputs.end(); ++iter) {
        tensorflow::TensorProto& result_tensor_proto = iter->second;
        int titer;
        for (titer = 0 ; titer !=result_tensor_proto.float_val_size(); ++titer){
          prediction.push_back(result_tensor_proto.float_val(titer));
        }
      }
      return prediction;
    } 
  
    else {
      std::cout << "gRPC call return code: " << status.error_code() << ": "
                << status.error_message() << std::endl;
      std::cout << "gRPC failed" << std::endl;
      return prediction;
    }
  }
};

static bool abs_compare(int a, int b)
{
    return (std::abs(a) < std::abs(b));
}



int main(int argc, char** argv) {
  
  const std::string path = "/home/dhaunss/workdir/datadir/txtfiles/";
  const std::string model_name = "best_model";
  const std::string model_signature_name = "predict";




    int i = 0;

  	std::string infile;
  	std::string outfile;


  	infile.append(path);
  	infile.append("noisy/noisy_");
  	infile.append(std::to_string(i));
  	infile.append(".txt");


  	std::ifstream is(infile);
 	  std::istream_iterator<float> start(is), end;
 	  std::vector<float> data (start, end);
    std::cout << "Loading file:" << infile << std::endl;
 	  std::cout << "Read " << data.size() << " numbers" << std::endl;




    std::vector<float>::iterator abs_val;
    abs_val = std::max_element(data.begin(),data.end(), abs_compare); 
    std::cout << "max Val =" << *abs_val << std::endl;



    for(int g= 0; g<data.size(); g++){
      //data[g] /= *abs_val;
      data[g] = 1;
    }    
    std::cout << "1??:   ";
    for(int g= 0; g<10; g++){
      std::cout << data[g];
    } std::cout << std::endl;

  	std::cout << "calling prediction service on " << "localhost:8500" << std::endl;
  	ServingClient sclient(grpc::CreateChannel("localhost:8500", grpc::InsecureChannelCredentials()));
  	std::vector<float> reply = sclient.callPredict(model_name, model_signature_name, data);


    for(int g = 0;g<reply.size(); g++){
      reply[g] *= *abs_val;
    }

    std::cout << "First 5 elements:";

    for(int n=0;n<5;n++){

      std::cout << reply[n];

    }
    std::cout << std::endl;

  	outfile.append(path);
  	outfile.append("denoised/denoised_trace_");
    outfile.append(std::to_string(i));
  	outfile.append(".txt");
    std::cout << "Write to file:" << outfile << std::endl
    <<  "______________________________________________" << std::endl;
  	std::ofstream file(outfile);


  	for (const auto &e : reply) file << e << std::endl;
  



  return 0;

} 
