#pragma once

#include <string>
#include <map>
#include <vector>

namespace FNX {

    class UserData {
    public:
        std::string username;
        std::string subscription;
        std::string expiry;
    };

    class ResponseData {
    public:
        bool success{};
        std::string message;
        std::string serverVersion;
        std::string username;
        std::string subscription;
        std::string expiry;
        std::string value;
        std::map<std::string, std::string> variables;
    };

    class api {
    private:
        struct ApiResponse {
            bool Success = false;
            std::string Message;
            std::string ServerVersion;
            std::string Username;
            std::string Subscription;
            std::string Expiry;
            std::string Value;
            std::map<std::string, std::string> Variables;
            std::string RequestNonce;
            std::string ServerTimestamp;
            std::string Signature;
        };

        std::string name, secret, version, url;
        std::string api_host;
        bool is_initialized = false;
        bool is_logged_in = false;
        UserData user_data;
        std::string saved_password;
        std::string saved_license;

        std::string api_path;
        std::string pinned_cert_sha256;
        std::string response_msg;

        std::wstring Utf8ToWide(const std::string& str);
        static std::string json_escape(const std::string& input);
        std::string extract_json(const std::string& json, const std::string& key);
        static bool extract_json_bool(const std::string& json, const std::string& key, bool defaultValue = false);
        static std::string extract_raw_json_object(const std::string& json, const std::string& key);
        static std::map<std::string, std::string> parse_string_map(const std::string& jsonObject);
        static std::string bytes_to_fingerprint(const unsigned char* bytes, unsigned long length);
        static std::string normalize_fingerprint(std::string value);
        void verify_pinned_certificate(void* hRequest) const;
        std::string build_request_json(const std::map<std::string, std::string>& fields) const;
        ApiResponse parse_response(const std::string& jsonStr);
        std::string http_request(const std::string& endpoint, const std::string& body);
        static std::string generate_nonce();
        static std::string get_timestamp();
        static std::string sha256_hex(const std::string& input);
        static std::vector<unsigned char> base64_decode(const std::string& input);
        static bool verify_signature(const std::string& payload, const std::string& base64Signature);
        std::string http_request(const std::string& endpoint, std::map<std::string, std::string> fields);

    public:
        std::map<std::string, std::string> variables;
        ResponseData response;

        api(std::string name, std::string secret, std::string version, std::string url);
        void pin_certificate(const std::string& sha256);
        void init();
        bool login(std::string username, std::string password);
        bool login_with_license(std::string license);
        void license(std::string key, std::string code = ""); // License-only login
        bool regstr(std::string username, std::string password, std::string license);
        bool changeUsername(std::string newusername);
        bool forgot(std::string username, std::string email);
        void logout();
        bool log(std::string message);
        void getvars();
        std::string Response();
        bool Initialized();
        UserData User();
    };
}
