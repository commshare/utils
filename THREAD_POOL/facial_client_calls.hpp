#ifndef FacialClientCalls_INCLUDED
#define FacialClientCalls_INCLUDED

#include <future>
#include <services/facial_service.pb.h>

namespace services_api
{
	struct AsyncFaceProcessCall : AbstractAsyncCall<Services::BiometricResponse> {
		void process() override
		{		
			auto gr = std::make_shared<Services::BiometricResponse>(response);
			promise.set_value(gr);
		}

		const std::string& identifier() const override
		{
			static std::string class_name = typeid(AsyncFaceProcessCall).name();
			return class_name;
		}

		std::promise<std::shared_ptr<Services::BiometricResponse>> promise;

		std::unique_ptr<grpc::ClientAsyncResponseReader<Services::BiometricResponse>> reader;
	};
}

#endif