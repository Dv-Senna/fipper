#pragma once

#include "fp/server.hpp"

#include <algorithm>


namespace fp {
	template <typename Func, typename RouteString>
	auto Server::m_addEndpoint(fp::HttpMethod method, RouteString route, Func &&callback) noexcept -> void {
		auto it {std::ranges::find_if(m_endpoints, [&route](const auto &endpoint){return endpoint.first->getString().size() >= route.getString().size();})};
		if (it == m_endpoints.end() || *it->first != route)
			it = m_endpoints.insert(it, {std::make_unique<RouteString> (route), {}});
		it->second[method] = std::make_unique<fp::Endpoint<Func>> (method, route, std::move(callback));
	}
} // namespace fp
