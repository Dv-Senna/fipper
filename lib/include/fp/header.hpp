#pragma once

#include <string>
#include <string_view>


namespace fp {
	enum class ContentType {
		eJson,
		eMpeg,
		eOgg,
		eAvif,
		eJpeg,
		ePng,
		eSvg,
		eTiff,
		eObj,
		ePlain,
		eCss,
		eCsv,
		eHtml,
		eJavascript,
		eXml,
		eUnknown
	};

	namespace header {
		struct RequestHeader {
			std::string_view host;
		};

		struct ResponseHeader {

		};

		struct RepresentationHeader {
			std::size_t contentSize;
			ContentType contentType {ContentType::eUnknown};
			std::string contentLanguage {"en"};
		};

	} // namespace header


	template <typename Body>
	struct RequestHeader : public header::RequestHeader, public header::RepresentationHeader {};
	template <>
	struct RequestHeader<void> : public header::RequestHeader {};

	template <typename Body>
	struct ResponseHeader : public header::ResponseHeader, public header::RepresentationHeader {};
	template <>
	struct ResponseHeader<void> : public header::ResponseHeader {};

} // namespace fp
