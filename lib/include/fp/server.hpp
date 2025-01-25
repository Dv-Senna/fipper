#pragma once

#include <atomic>
#include <latch>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "fp/core.hpp"
#include "fp/endpoint.hpp"
#include "fp/httpMethod.hpp"
#include "fp/result.hpp"
#include "fp/routeString.hpp"
#include "fp/socket.hpp"


namespace fp {


	class FP_CORE Server {
		public:
			struct CreateInfos {
				std::uint16_t port;
				std::size_t latchsReservedCount {16};
				std::size_t socketQueueSizeHint {16};
			};

			Server() noexcept = default;
			~Server() = default;

			[[nodiscard]] auto create(const CreateInfos &createInfos) noexcept -> fp::Result;
			[[nodiscard]] auto run() noexcept -> fp::Result;

			template <typename Func>
			auto get(typename fp::utils::SanitizedFunctionParameter_t<Func, 0>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::eGet, route, std::move(callback));
			}

			template <typename Func>
			auto post(typename fp::utils::SanitizedFunctionParameter_t<Func, 0>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::ePost, route, std::move(callback));
			}

			template <typename Func>
			auto put(typename fp::utils::SanitizedFunctionParameter_t<Func, 0>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::ePut, route, std::move(callback));
			}

			template <typename Func>
			auto patch(typename fp::utils::SanitizedFunctionParameter_t<Func, 0>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::ePatch, route, std::move(callback));
			}

			template <typename Func>
			auto delete_(typename fp::utils::SanitizedFunctionParameter_t<Func, 0>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::eDelete, route, std::move(callback));
			}


		private:
			template <typename Func, typename RouteString>
			auto m_addEndpoint(fp::HttpMethod method, RouteString route, Func &&callback) noexcept -> void;

			auto m_getLatch() noexcept -> std::shared_ptr<std::latch>;
			auto m_handleRequest(fp::Socket &&connection, std::string request) noexcept -> void;

			static auto s_signalHandler(int signal) noexcept -> void;
			static std::atomic_bool s_endSignal;

			std::uint16_t m_port;
			std::size_t m_socketQueueSizeHint;
			std::vector<std::pair<std::unique_ptr<fp::RouteStringBase>, std::map<fp::HttpMethod, std::unique_ptr<fp::EndpointBase>>>> m_endpoints;
			std::vector<std::optional<std::shared_ptr<std::latch>>> m_latchs;
			fp::Socket m_serverSocket;
	};

} // namespace fp

#include "fp/server.inl"
