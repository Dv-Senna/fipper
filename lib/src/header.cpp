#include "fp/header.hpp"


namespace fp {
	auto getContentTypeString(ContentType type) noexcept -> std::string_view {
		switch (type) {
			case ContentType::eJson: return "application/json";
			case ContentType::eMpeg: return "audio/mpeg";
			case ContentType::eOgg: return "audio/ogg";
			case ContentType::eAvif: return "image/avif";
			case ContentType::eJpeg: return "image/jpeg";
			case ContentType::ePng: return "image/png";
			case ContentType::eSvg: return "image/svg+xml";
			case ContentType::eTiff: return "image/tiff";
			case ContentType::eObj: return "model/obj";
			case ContentType::ePlain: return "text/plain";
			case ContentType::eCss: return "text/css";
			case ContentType::eCsv: return "text/csv";
			case ContentType::eHtml: return "text/html";
			case ContentType::eJavascript: return "text/javascript";
			case ContentType::eXml: return "text/xml";
			case ContentType::eUnknown: return "unknown";
		}

		return "text/plain";
	}

} // namespace fp
