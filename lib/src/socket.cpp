#include "fp/socket.hpp"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


namespace fp {
	Socket::~Socket() {
		if (m_socket > 0)
			close(m_socket);
	}


	auto Socket::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		m_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == -1)
			return fp::Result::eFailure;

		sockaddr_in6 serverAddress {};
		serverAddress.sin6_family = AF_INET6;
		serverAddress.sin6_port = htons(createInfos.port);
		serverAddress.sin6_addr = IN6ADDR_LOOPBACK_INIT;
		if (bind(m_socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
			return fp::Result::eFailure;

		return fp::Result::eSuccess;
	}

} // namespace fp
