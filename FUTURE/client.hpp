#include <chrono>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>

#include "jobs/managed_job.hpp"
#include "jobs/sequence.hpp"

#include "dbg/grpc/tenncor.grpc.pb.h"

#ifndef DBG_GRPC_CLIENT_HPP
#define DBG_GRPC_CLIENT_HPP

namespace dbg
{

static const size_t max_attempts = 10;

static const size_t data_sync_interval = 50;

struct ClientConfig
{
	ClientConfig (void) = default;

	ClientConfig (std::chrono::duration<int64_t,std::milli> request_duration,
		std::chrono::duration<int64_t,std::milli> stream_duration) :
		request_duration_(request_duration), stream_duration_(stream_duration) {}

	std::chrono::duration<int64_t,std::milli> request_duration_ =
		std::chrono::milliseconds(250);

	std::chrono::duration<int64_t,std::milli> stream_duration_ =
		std::chrono::milliseconds(10000);
};

struct GraphEmitterClient final
{
	GraphEmitterClient (std::shared_ptr<grpc::ChannelInterface> channel,
		ClientConfig cfg) :
		stub_(tenncor::GraphEmitter::NewStub(channel)),
		cfg_(cfg),
		connected_(true)
	{
		jobs::ManagedJob healthjob(
		[this](std::future<void> stop_it)
		{
			tenncor::Empty empty;
			do
			{
				grpc::ClientContext context;
				tenncor::CreateGraphResponse response;
				// set context deadline
				std::chrono::time_point<std::chrono::system_clock> deadline =
					std::chrono::system_clock::now() +
					std::chrono::milliseconds(1000);
				context.set_deadline(deadline);
				grpc::Status status =
					stub_->HealthCheck(&context, empty, &empty);
				this->connected_ = status.ok();

				std::this_thread::sleep_for(
					std::chrono::milliseconds(1000));
			}
			while (stop_it.wait_for(std::chrono::milliseconds(1)) ==
				std::future_status::timeout);
		});
		health_checker_ = std::move(healthjob);
	}

	/// Add job that pass CreateGraphRequest
	void create_graph (tenncor::CreateGraphRequest& request)
	{
		// retries sending creation request unless stop_it times out
		sequential_jobs_.attach_job(
		[this](std::future<void> dependency, std::future<void> stop_it,
			tenncor::CreateGraphRequest request)
		{
			if (dependency.valid())
			{
				dependency.get(); // wait for dependency completion
			}
			std::string sid = fmts::to_string(
				std::this_thread::get_id());
			for (size_t attempt = 0;
				stop_it.wait_for(std::chrono::milliseconds(1)) ==
				std::future_status::timeout && attempt < max_attempts;
				++attempt)
			{
				grpc::ClientContext context;
				tenncor::CreateGraphResponse response;
				// set context deadline
				std::chrono::time_point<std::chrono::system_clock> deadline =
					std::chrono::system_clock::now() + cfg_.request_duration_;
				context.set_deadline(deadline);

				grpc::Status status = this->stub_->CreateGraph(
					&context, request, &response);
				if (status.ok())
				{
					auto res_status = response.status();
					if (tenncor::Status::OK != res_status)
					{
						logs::errorf("%s: %s",
							tenncor::Status_Name(res_status).c_str(),
							response.message().c_str());
					}
					else
					{
						logs::infof("%s: CreateGraphRequest success: %s",
							sid.c_str(), response.message().c_str());
						return;
					}
				}
				else
				{
					logs::errorf(
						"%s: CreateGraphRequest attempt %d failure: %s",
						sid.c_str(), attempt,
						status.error_message().c_str());
				}
				std::this_thread::sleep_for(
					std::chrono::milliseconds(attempt * 1000));
			}
			logs::warnf("%s: CreateGraphRequest terminating", sid.c_str());
		}, std::move(request));
	}

	/// Add job that pass UpdateGraphRequest
	void update_graph (tenncor::UpdateGraphRequest& request)
	{
		// retries sending creation request unless stop_it times out
		sequential_jobs_.attach_job(
		[this](std::future<void> dependency, std::future<void> stop_it,
			tenncor::UpdateGraphRequest request)
		{
			if (dependency.valid())
			{
				dependency.get(); // wait for dependency completion
			}
			std::string sid = fmts::to_string(
				std::this_thread::get_id());
			for (size_t attempt = 0;
				stop_it.wait_for(std::chrono::milliseconds(1)) ==
				std::future_status::timeout && attempt < max_attempts;
				++attempt)
			{
				grpc::ClientContext context;
				tenncor::UpdateGraphResponse response;
				// set context deadline
				std::chrono::time_point<std::chrono::system_clock> deadline =
					std::chrono::system_clock::now() + cfg_.request_duration_;
				context.set_deadline(deadline);

				grpc::Status status = this->stub_->UpdateGraph(
					&context, request, &response);
				if (status.ok())
				{
					auto res_status = response.status();
					if (tenncor::Status::OK != res_status)
					{
						logs::errorf("%s: %s",
							tenncor::Status_Name(res_status).c_str(),
							response.message().c_str());
					}
					else
					{
						logs::infof("%s: UpdateGraphRequest success: %s",
							sid.c_str(), response.message().c_str());
						return;
					}
				}
				else
				{
					logs::errorf(
						"%s: UpdateGraphRequest attempt %d failure: %s",
						sid.c_str(), attempt,
						status.error_message().c_str());
				}
				std::this_thread::sleep_for(
					std::chrono::milliseconds(attempt * 1000));
			}
			logs::warnf("%s: UpdateGraphRequest terminating", sid.c_str());
		}, std::move(request));
	}

	/// Add job that streams UpdateNodeDataRequest
	void update_node_data (
		std::vector<tenncor::UpdateNodeDataRequest>& requests,
		size_t update_it)
	{
		sequential_jobs_.attach_job(
		[this](std::future<void> dependency,
			std::future<void> stop_it,
			std::vector<tenncor::UpdateNodeDataRequest> requests, size_t update_it)
		{
			if (dependency.valid())
			{
				dependency.get(); // wait for dependency completion
			}
			std::string sid = fmts::to_string(
				std::this_thread::get_id());
			tenncor::UpdateNodeDataResponse response;
			grpc::ClientContext context;
			// set context deadline
			std::chrono::time_point<std::chrono::system_clock> deadline =
				std::chrono::system_clock::now() +
				std::chrono::milliseconds(cfg_.stream_duration_);
			context.set_deadline(deadline);
			std::unique_ptr<grpc::ClientWriterInterface<
				tenncor::UpdateNodeDataRequest>> writer(
				stub_->UpdateNodeData(&context, &response));

			for (auto& request : requests)
			{
				if (stop_it.wait_for(std::chrono::milliseconds(1)) !=
					std::future_status::timeout)
				{
					break;
				}
				if (false == writer->Write(request))
				{
					logs::errorf("failed to write update %d", update_it);
					break;
				}
			}
			writer->WritesDone();

			grpc::Status status = writer->Finish();
			if (status.ok())
			{
				auto res_status = response.status();
				if (tenncor::Status::OK != res_status)
				{
					logs::errorf("%s: %s",
						tenncor::Status_Name(res_status).c_str(),
						response.message().c_str());
				}
				else
				{
					return;
				}
			}
			else
			{
				logs::errorf(
					"UpdateNodeData failure: %s",
					status.error_message().c_str());
			}
			logs::warnf("%s: UpdateNodeData terminating", sid.c_str());
		}, std::move(requests), std::move(update_it));
	}

	bool is_connected (void)
	{
		return connected_;
	}

	void join (void)
	{
		sequential_jobs_.join();
	}

	void clear (void)
	{
		sequential_jobs_.stop();
	}

private:
	std::unique_ptr<tenncor::GraphEmitter::Stub> stub_;

	ClientConfig cfg_;

	// every request from emitter has dependency on the previous request
	jobs::Sequence sequential_jobs_;

	// connection state
	std::atomic<bool> connected_;
	jobs::ManagedJob health_checker_;
};

}

#endif // DBG_GRPC_CLIENT_HPP
