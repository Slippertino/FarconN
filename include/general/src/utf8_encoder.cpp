#include "../tools/utf8_encoder.hpp"

FARCONN_NAMESPACE_BEGIN(general)

void utf8_encoder::from_local_to_utf8(std::string& source) {
	std::wstring src_uni; 
	to_unicode(source, src_uni, CP_ACP);
	from_unicode(src_uni, source, CP_UTF8);
}

void utf8_encoder::from_utf8_to_local(std::string& source) {
	std::wstring src_uni;
	to_unicode(source, src_uni, CP_UTF8);
	from_unicode(src_uni, source, CP_ACP);
	fix_inaccuracies_local(source);
}

void utf8_encoder::from_local_to_utf8_list(const std::initializer_list<std::string*>& sources) {
	for (auto& src : sources) {
		from_local_to_utf8(*src);
	}
}

void utf8_encoder::from_utf8_to_local_list(const std::initializer_list<std::string*>& sources) {
	for (auto src : sources) {
		from_utf8_to_local(*src);
	}
}

void utf8_encoder::to_unicode(const std::string& source, std::wstring& result, UINT code_page) {
	int size = MultiByteToWideChar(code_page, MB_COMPOSITE, source.c_str(), source.length(), nullptr, 0);

	result = std::wstring(size, '\0');

	MultiByteToWideChar(code_page, MB_COMPOSITE, source.c_str(), source.length(), &result[0], size);
}

void utf8_encoder::from_unicode(const std::wstring& source, std::string& result, UINT code_page) {
	int size = WideCharToMultiByte(code_page, 0, source.c_str(), source.length(), nullptr, 0, nullptr, nullptr);

	result = std::string(size, '\0');

	WideCharToMultiByte(code_page, 0, source.c_str(), source.length(), &result[0], size, nullptr, nullptr);
}

void utf8_encoder::fix_inaccuracies_local(std::string& source) {
	std::string old_sub = "è?", new_sub = "é";

	size_t pos;
	while ((pos = source.find(old_sub)) != std::string::npos) {
		source.replace(pos, old_sub.size(), new_sub);
	}
}

FARCONN_NAMESPACE_END