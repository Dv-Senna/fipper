#pragma once

#include <atomic>
#include <map>
#include <memory>

#include "fp/core.hpp"
#include "fp/endpoint.hpp"
#include "fp/httpMethod.hpp"
#include "fp/result.hpp"
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
			auto get(typename fp::utils::SanitizeParameter_t<fp::utils::FunctionParamater_t<Func, 0>>::Route route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::eGet, route, std::move(callback));
			}


		private:
			template <typename Func, typename RouteString>
			auto m_addEndpoint(fp::HttpMethod method, RouteString route, Func &&callback) noexcept -> void {
				m_endpoints[route.getString()][method] = std::make_unique<fp::Endpoint<Func>> (method, route, std::move(callback));
			}

			static auto s_signalHandler(int signal) noexcept -> void;
			static std::atomic_bool s_endSignal;

			std::uint16_t m_port;
			std::map<std::string_view, std::map<fp::HttpMethod, std::unique_ptr<fp::EndpointBase>>> m_endpoints;
			fp::Socket m_serverSocket;
	};

} // namespace fp
