#include "fp/httpCode.hpp"


namespace fp {
	std::optional<std::string_view> getHttpCodeString(HttpCode code) {
		switch (code) {
			case HttpCode::e100: return "Continue";
			case HttpCode::e101: return "Switching Protocols";
			case HttpCode::e102: return "Processing";
			case HttpCode::e200: return "OK";
			case HttpCode::e201: return "Created";
			case HttpCode::e202: return "Accepted";
			case HttpCode::e203: return "Non-authoritative Information";
			case HttpCode::e204: return "No Content";
			case HttpCode::e205: return "Reset Content";
			case HttpCode::e206: return "Partial Content";
			case HttpCode::e207: return "Multi-Status";
			case HttpCode::e208: return "Already Reported";
			case HttpCode::e226: return "IM Used";
			case HttpCode::e300: return "Multiple Choices";
			case HttpCode::e301: return "Moved Permanently";
			case HttpCode::e302: return "Found";
			case HttpCode::e303: return "See Other";
			case HttpCode::e304: return "Not Modified";
			case HttpCode::e307: return "Temporary Redirect";
			case HttpCode::e308: return "Permanent Redirect";
			case HttpCode::e400: return "Bad Request";
			case HttpCode::e401: return "Unauthorized";
			case HttpCode::e402: return "Payment Required";
			case HttpCode::e403: return "Forbidden";
			case HttpCode::e404: return "Not Found";
			case HttpCode::e405: return "Method Not Allowed";
			case HttpCode::e406: return "Not Acceptable";
			case HttpCode::e407: return "Proxy Authentication Required";
			case HttpCode::e408: return "Request Timeout";
			case HttpCode::e409: return "Conflict";
			case HttpCode::e410: return "Gone";
			case HttpCode::e411: return "Length Required";
			case HttpCode::e412: return "Precondition Failed";
			case HttpCode::e413: return "Payload Too Large";
			case HttpCode::e414: return "Request-URI Too Long";
			case HttpCode::e415: return "Unsupported Media Type";
			case HttpCode::e416: return "Requested Range Not Satisfiable";
			case HttpCode::e417: return "Expectation Failed";
			case HttpCode::e418: return "I'm a teapot";
			case HttpCode::e421: return "Misdirected Request";
			case HttpCode::e422: return "Unprocessable Entity";
			case HttpCode::e423: return "Locked";
			case HttpCode::e424: return "Failed Dependency";
			case HttpCode::e426: return "Upgrade Required";
			case HttpCode::e428: return "Precondition Required";
			case HttpCode::e429: return "Too Many Requests";
			case HttpCode::e431: return "Request Header Fields Too Large";
			case HttpCode::e444: return "Connection Closed Without Response";
			case HttpCode::e451: return "Unavailable For Legal Reasons";
			case HttpCode::e499: return "Client Closed Request";
			case HttpCode::e500: return "Internal Server Error";
			case HttpCode::e501: return "Not Implemented";
			case HttpCode::e502: return "Bad Gateway";
			case HttpCode::e503: return "Service Unavailable";
			case HttpCode::e504: return "Gateway Timeout";
			case HttpCode::e505: return "HTTP Version Not Supported";
			case HttpCode::e506: return "Variant Also Negotiates";
			case HttpCode::e507: return "Insufficient Storage";
			case HttpCode::e508: return "Loop Detected";
			case HttpCode::e510: return "Not Extended";
			case HttpCode::e511: return "Network Authentication Required";
			case HttpCode::e599: return "Network Connect Timeout Error";
		}

		return std::nullopt;
	}

} // namespace fp
