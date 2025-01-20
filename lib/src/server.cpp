#include "fp/server.hpp"


namespace fp {
	auto Server::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		fp::Socket::CreateInfos socketCreateInfos {};
		socketCreateInfos.port = createInfos.port;
		if (m_serverSocket.create(socketCreateInfos) != fp::Result::eSuccess)
			return fp::Result::eFailure;

		return fp::Result::eSuccess;
	}


	auto Server::run() noexcept -> fp::Result {
		return fp::Result::eSuccess;
	}

} // namespace fp
