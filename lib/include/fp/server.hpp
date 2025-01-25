#pragma once

#include <atomic>
#include <map>
#include <memory>
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

			auto m_handleRequest(fp::Socket &&connection, std::string request) const noexcept -> void;

			static auto s_signalHandler(int signal) noexcept -> void;
			static std::atomic_bool s_endSignal;

			std::uint16_t m_port;
			std::vector<std::pair<std::unique_ptr<fp::RouteStringBase>, std::map<fp::HttpMethod, std::unique_ptr<fp::EndpointBase>>>> m_endpoints;
			fp::Socket m_serverSocket;
	};

} // namespace fp

#include "fp/server.inl"
