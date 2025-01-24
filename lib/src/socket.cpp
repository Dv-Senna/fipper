#include "fp/socket.hpp"

#include <netdb.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <print>

#include "fp/errorStack.hpp"


namespace fp {
	Socket::Socket(Socket &&socket) noexcept :
		m_socket {socket.m_socket}
	{
		socket.m_socket = 0;
	}


	Socket &Socket::operator=(Socket &&socket) noexcept {
		this->~Socket();
		m_socket = socket.m_socket;
		socket.m_socket = 0;
		return *this;
	}


	Socket::~Socket() {
		if (m_socket > 0)
			close(m_socket);
	}


	auto Socket::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		m_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == -1)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't create socket");

		int reuse {1};
		if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't set socket to reuse");

		sockaddr_in6 serverAddress {};
		serverAddress.sin6_family = AF_INET6;
		serverAddress.sin6_port = htons(createInfos.port);
		serverAddress.sin6_addr = IN6ADDR_LOOPBACK_INIT;
		if (bind(m_socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't bind socket");

		return fp::Result::eSuccess;
	}


	auto Socket::listen(std::uint32_t queueSizeHint) noexcept -> fp::Result {
		if (::listen(m_socket, queueSizeHint) != 0)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't listen socket");
		return fp::Result::eSuccess;
	}


	auto Socket::accept(std::chrono::milliseconds timeout) noexcept -> std::expected<Socket, fp::Result> {
		auto dataToRecieve {this->hasDataToRecieve(timeout)};
		if (!dataToRecieve)
			return fp::ErrorStack::push(std::unexpected(fp::Result::eFailure), "Can't check for socket to accept");
		if (!*dataToRecieve)
			return std::unexpected(fp::Result::eNothingToAccept);

		Socket socket {};
		socket.m_socket = ::accept(m_socket, nullptr, nullptr);
		if (socket.m_socket == -1)
			return fp::ErrorStack::push(std::unexpected(fp::Result::eFailure), "Can't accept socket");
		return socket;
	}


	auto Socket::hasDataToRecieve(std::chrono::milliseconds timeout) noexcept -> std::optional<bool> {
		pollfd fd {};
		fd.fd = m_socket;
		fd.events = POLLIN;

		if (poll(&fd, 1, timeout.count()) < 0) {
			if (errno == EINTR)
				return false;
			return fp::ErrorStack::push(std::nullopt, "Can't poll socket for reading");
		}
		if (fd.revents & POLLERR || fd.revents & POLLHUP || fd.revents & POLLNVAL)
			return fp::ErrorStack::push(std::nullopt, "Can't poll socket for reading");
		return fd.revents & POLLIN;
	}


	auto Socket::recieve(std::byte *buffer, std::size_t bufferSize) noexcept -> std::expected<std::vector<std::byte>, fp::Result> {
		auto dataToRecieve {this->hasDataToRecieve(1000ms)};
		if (!dataToRecieve)
			return fp::ErrorStack::push(std::unexpected(fp::Result::eFailure), "Can't check for recievablity of socket before recieving");
		if (!*dataToRecieve)
			return std::unexpected(fp::Result::eNothingToRecieve);

		std::vector<std::byte> data {};
		ssize_t written {};
		do {
			written = recv(m_socket, buffer, bufferSize, 0);
			if (written < 0)
				return fp::ErrorStack::push(std::unexpected(fp::Result::eFailure), "Can't recieve data from socket");
			data.insert(data.end(), buffer, buffer + written);
			dataToRecieve = this->hasDataToRecieve();
			if (!dataToRecieve)
				return fp::ErrorStack::push(std::unexpected(fp::Result::eFailure), "Can't check if more data is available in socket for recieving");
		} while (*dataToRecieve);

		return data;
	}


	auto Socket::send(const std::vector<std::byte> &data) noexcept -> fp::Result {
		if (::send(m_socket, data.data(), data.size(), 0) != static_cast<ssize_t> (data.size()))
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't send data to socket");
		return fp::Result::eSuccess;
	}

} // namespace fp
