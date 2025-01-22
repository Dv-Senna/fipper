#pragma once

#include <latch>
#include <memory>
#include <string_view>

#include "fp/httpMethod.hpp"
#include "fp/result.hpp"
#include "fp/socket.hpp"


namespace fp {
	class EndpointBase {
		public:
			EndpointBase() noexcept = delete;
			constexpr EndpointBase(fp::HttpMethod method, std::string_view route) noexcept : m_method {method}, m_route {route} {}
			~EndpointBase() = default;

			virtual auto handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view requestString) noexcept -> fp::Result = 0;

		protected:
			fp::HttpMethod m_method;
			std::string_view m_route;
	};


	template <typename Request, typename Response, typename Func>
	class Endpoint final : public EndpointBase {
		public:
			Endpoint() noexcept = delete;
			constexpr Endpoint(fp::HttpMethod method, std::string_view route, Func &&callback) noexcept;
			~Endpoint() = default;

			 auto handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view requestString) noexcept -> fp::Result override;


		private:
			Func &&m_callback;
	};

} // namespace fp

#include "fp/endpoint.inl"
