#pragma once

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
			auto get(std::string_view route, Func &&callback) noexcept -> void {
				this->m_addEndpoint<Func> (fp::HttpMethod::eGet, route, std::move(callback));
			}


		private:
			template <typename Func>
			auto m_addEndpoint(fp::HttpMethod method, std::string_view route, Func &&callback) noexcept -> void {
				m_endpoints[method][route] = std::make_unique<fp::Endpoint<Func>> (method, route, std::move(callback));
			}

			static auto s_signalHandler(int signal) noexcept -> void;
			static bool s_endSignal;

			std::uint16_t m_port;
			std::map<fp::HttpMethod, std::map<std::string_view, std::unique_ptr<fp::EndpointBase>>> m_endpoints;
			fp::Socket m_serverSocket;
	};

} // namespace fp
