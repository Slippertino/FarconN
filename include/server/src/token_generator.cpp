#include "../tools/token_generator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

std::string token_generator::generate(std::function<bool(const std::string&)> predicate) {
    std::string result;

    do {
        UUID uuid;
        ZeroMemory(&uuid, sizeof(UUID));

        UuidCreate(&uuid);
        CHAR* pUuidString = NULL;
        UuidToStringA(&uuid, (RPC_CSTR*)&pUuidString);

        if (pUuidString != NULL)
        {
            result = pUuidString;
        }

        RpcStringFreeA((RPC_CSTR*)&pUuidString);
    } while (!predicate(result));

    return result;
}

FARCONN_NAMESPACE_END