#ifndef __ASYNC_SET_HPP__
#define __ASYNC_SET_HPP__

#include <grpc++/grpc++.h>
#include "proto/rpc.grpc.pb.h"
#include "v3/include/Action.hpp"
#include "v3/include/AsyncTxnResponse.hpp"


using grpc::ClientAsyncResponseReader;
using etcdserverpb::TxnResponse;
using etcdserverpb::KV;

namespace etcdv3
{
  class AsyncSetAction : public etcdv3::Action
  {
    public:
      AsyncSetAction(std::string const & key, std::string const & value, KV::Stub* stub_, bool create=false);
      AsyncTxnResponse ParseResponse();
      TxnResponse reply;
      std::unique_ptr<ClientAsyncResponseReader<TxnResponse>> response_reader;
      bool isCreate;
  };
}

#endif
