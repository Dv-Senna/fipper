#include "fp/server.hpp"

#include <csignal>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <print>
#include <unistd.h>

#include "fp/errorStack.hpp"
#include "fp/utils/benchmark.hpp"


#define FP_REQUEST_DISPATCHING_BENCHMARK fp::utils::Benchmark<"request_dispatching"_hash>
#define FP_REQUEST_HANDLING_BENCHMARK fp::utils::Benchmark<"request_dispatching"_hash, std::string>

#ifndef FP_BENCHMARK_ENABLED
namespace fp::utils {
	using namespace fp::utils::literals;

	template <>
	struct BenchmarkEnabled<"request_dispatching"_hash> : std::false_type {};
	template <>
	struct BenchmarkEnabled<"request_handling"_hash> : std::false_type {};
};
#endif // FP_BENCHMARK_ENABLED


namespace fp {
	auto Server::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		m_port = createInfos.port;
		m_socketQueueSizeHint = createInfos.socketQueueSizeHint;

		fp::JobScheduler::CreateInfos jobSchedulerCreateInfos {};
		jobSchedulerCreateInfos.minThreadCount = createInfos.minWorkThreadCount;
		if (m_jobScheduler.create(jobSchedulerCreateInfos) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't create job scheduler");

		fp::Socket::CreateInfos socketCreateInfos {};
		socketCreateInfos.port = m_port;
		if (m_serverSocket.create(socketCreateInfos) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't create server socket");

		return fp::Result::eSuccess;
	}


	auto Server::run() noexcept -> fp::Result {
		using namespace fp::utils::literals;
		using namespace std::literals;

		FP_REQUEST_DISPATCHING_BENCHMARK::setCallback([](std::chrono::microseconds duration) {
			static std::mutex mutex {};
			static std::ofstream csv {"benchmarks/request_dispatching.csv"};

			std::lock_guard _ {mutex};
			csv << duration.count() << std::endl;
		});

		FP_REQUEST_HANDLING_BENCHMARK::setCallback([](std::chrono::microseconds duration, const std::string &route) {
			static std::mutex mutex {};
			static std::ofstream csv {"benchmarks/request_handling.csv"};

			std::lock_guard _ {mutex};
			csv << std::quoted(route) << "," << duration.count() << std::endl;
		});


		(void)std::signal(SIGINT, Server::s_signalHandler);

		if (m_serverSocket.listen(m_socketQueueSizeHint) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't listen to server socket");

		std::println("Server listening");
		std::println("\tFor local use : \033]8;;http://localhost:{0}\033\\localhost:{0}\033]8;;\033\\", m_port);

		while (!s_endSignal) {
			auto clientSocketWithError {m_serverSocket.accept(1000ms)};
			if (!clientSocketWithError) {
				if (clientSocketWithError.error() == fp::Result::eNothingToAccept)
					continue;
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't accept client socket");
			}

			fp::Socket &clientSocket {*clientSocketWithError};
			FP_REQUEST_DISPATCHING_BENCHMARK _ {};

			auto hasDataToRecieveWithError {clientSocket.hasDataToRecieve(1000ms)};
			if (!hasDataToRecieveWithError)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't check read data for client socket");
			if (!*hasDataToRecieveWithError)
				continue;

			auto requestWithError {clientSocket.recieve()};
			if (!requestWithError)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't read data for client socket");

			if (this->m_handleRequest(
				std::move(clientSocket),
				std::string{(char*)requestWithError->data(), (char*)requestWithError->data() + requestWithError->size()}
			) != fp::Result::eSuccess)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't handle request");
		}

		return fp::Result::eSuccess;
	}


	auto Server::m_handleRequest(fp::Socket &&connection, std::string request) noexcept -> fp::Result {
		using namespace fp::utils::literals;

		fp::Job job {[this, clientSocket = std::move(connection), request = std::move(request)]() mutable noexcept -> fp::Result {
			static const std::map<std::string_view, fp::HttpMethod> methodMap {
				{"GET", fp::HttpMethod::eGet},
				{"PUT", fp::HttpMethod::ePut},
				{"POST", fp::HttpMethod::ePost},
				{"PATCH", fp::HttpMethod::ePatch},
				{"DELETE", fp::HttpMethod::eDelete}
			};

			FP_REQUEST_HANDLING_BENCHMARK benchmark {};

			auto split {std::views::split(request, ' ')};
			std::string_view methodString {*split.begin()};
			auto method {methodMap.find(methodString)};
			if (method == methodMap.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 400 Bad Request"sv)->data) != fp::Result::eSuccess)
					return fp::ErrorStack::push(fp::Result::eFailure, "Can't send 400 after invalid method name");
				return fp::Result::eSuccess;
			}

			std::string_view routeString {*++split.begin()};
			benchmark.setArgs(std::string{routeString});
			auto route {std::ranges::find_if(m_endpoints, [&routeString](const auto &endpoint){return endpoint.first->isInstance(routeString);})};
			if (route == m_endpoints.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 404 Not Found"sv)->data) != fp::Result::eSuccess)
					return fp::ErrorStack::push(fp::Result::eFailure, "Can't send 404 after invalid route");
				return fp::Result::eSuccess;
			}

			auto endpoint {route->second.find(method->second)};
			if (endpoint == route->second.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 405 Method Not Allowed"sv)->data) != fp::Result::eSuccess)
					return fp::ErrorStack::push(fp::Result::eFailure, "Can't send 405 after invalid method");
				return fp::Result::eSuccess;
			}

			endpoint->second->handleRequest(std::move(clientSocket), request);
			return fp::Result::eSuccess;
		}};

		fp::JobMetadata jobMetadata {};
		jobMetadata.priority = fp::JobPriority::eMedium;

		fp::JobPushingLockGuard pushingLock {};
		if (fp::JobQueue::push(jobMetadata, std::move(job)) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't push job to job queue");
		return fp::Result::eSuccess;
	}


	auto Server::s_signalHandler(int) noexcept -> void {
		s_endSignal = true;
	}


	std::atomic_bool Server::s_endSignal {false};
} // namespace fp
