#include "fp/socket.hpp"

#include <netdb.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


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
			return fp::Result::eFailure;

		sockaddr_in6 serverAddress {};
		serverAddress.sin6_family = AF_INET6;
		serverAddress.sin6_port = htons(createInfos.port);
		serverAddress.sin6_addr = IN6ADDR_LOOPBACK_INIT;
		if (bind(m_socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
			return fp::Result::eFailure;

		return fp::Result::eSuccess;
	}


	auto Socket::listen(std::uint32_t queueSizeHint) noexcept -> fp::Result {
		if (::listen(m_socket, queueSizeHint) != 0)
			return fp::Result::eFailure;
		return fp::Result::eSuccess;
	}


	auto Socket::accept() noexcept -> std::optional<Socket> {
		Socket socket {};
		socket.m_socket = ::accept(m_socket, nullptr, nullptr);
		if (socket.m_socket == -1)
			return std::nullopt;
		return socket;
	}


	auto Socket::hasDataToRecieve(std::chrono::milliseconds timeout) noexcept -> std::optional<bool> {
		pollfd fd {};
		fd.fd = m_socket;
		fd.events = POLLIN;

		if (poll(&fd, 1, timeout.count()) < 0)
			return std::nullopt;
		if (fd.revents & POLLERR || fd.revents & POLLHUP || fd.revents & POLLNVAL)
			return std::nullopt;
		return fd.revents & POLLIN;
	}


	auto Socket::recieve(std::byte *buffer, std::size_t bufferSize) noexcept -> std::expected<std::vector<std::byte>, fp::Result> {
		auto dataToRecieve {this->hasDataToRecieve(1000ms)};
		if (!dataToRecieve)
			return std::unexpected(fp::Result::eFailure);
		if (!*dataToRecieve)
			return std::unexpected(fp::Result::eNothingToRecieve);

		std::vector<std::byte> data {};
		ssize_t written {};
		do {
			written = recv(m_socket, buffer, bufferSize, 0);
			if (written < 0)
				return std::unexpected(fp::Result::eFailure);
			data.insert(data.end(), buffer, buffer + written);
			dataToRecieve = this->hasDataToRecieve();
			if (!dataToRecieve)
				return std::unexpected(fp::Result::eFailure);
		} while (*dataToRecieve);

		return data;
	}


	auto Socket::send(const std::vector<std::byte> &data) noexcept -> fp::Result {
		if (::send(m_socket, data.data(), data.size(), 0) != static_cast<ssize_t> (data.size()))
			return fp::Result::eFailure;
		return fp::Result::eSuccess;
	}

} // namespace fp