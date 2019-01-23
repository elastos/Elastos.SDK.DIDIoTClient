#ifndef _ELASTOS_SDK_DID_IOTCLIENT_HPP_
#define _ELASTOS_SDK_DID_IOTCLIENT_HPP_

#include <map>
#include <string>
#include <vector>

namespace elastos {

class ElastosSdkDidIotApi {
    public:
        /*** type define ***/
        class ErrCode {
            public:
                static constexpr int Unknown           = -1;
                static constexpr int NotFound          = -2;
                static constexpr int NullArgument      = -3;
                static constexpr int BadArgument       = -4;
                static constexpr int MnemonicNotExists = -5;
                static constexpr int UserCanceled      = -6;
                static constexpr int IOFailed          = -7;
                static constexpr int NetFailed         = -8;
                static constexpr int WalletLibError    = -1000;
                static constexpr int HttpClientError   = -2000;
            private:
                explicit ErrCode() = delete;
                virtual ~ErrCode() = delete;
        };

        /*** static function and variable ***/
        static int GenerateMnemonic(std::string& mnemonic);

        /*** class function and variable ***/
        explicit ElastosSdkDidIotApi();
        virtual ~ElastosSdkDidIotApi();

        int revertMnemonic(const std::string& mnemonic);
        int getDeviceDid(std::string& did) const;
        int setProperty(const std::string& key, const std::string& value);
        int uploadToBlockchain();

        int setAgentUrl(const std::string& agentUrl);
        int setAgentIdAndKey(const std::string& appId, const std::string& appKey);

    protected:
        /*** type define ***/

        /*** static function and variable ***/

        /*** class function and variable ***/

    private:
        /*** type define ***/
        static constexpr const char* DefaultAgentURL     = "https://api-wallet-did.elastos.org/api/1/blockagent/upchain/data";
        static constexpr const char* SchemeHttp          = "http://";
        static constexpr const char* SchemeHttps         = "https://";
        static constexpr const char* MnemonicLanguage    = "english";
        static constexpr const char* AgentAuthHeaderName = "X-Elastos-Agent-Auth";

        /*** static function and variable ***/
        int getSerialProperties(const std::map<std::string, std::string>& props,
                                std::string& serialProps) const;
        int getSignedProperties(const std::vector<uint8_t>& seed,
                                const std::string& serialProps,
                                std::string& signedProps) const;
        int formatAgentProperties(const std::vector<uint8_t>& seed,
                                  const std::string& serialProps,
                                  const std::string& signedProps,
                                  std::string& formatProps) const;
        int getAgentAuthValue(std::string& authValue) const;

        /*** class function and variable ***/
        std::string mAgentUrl;
        std::string mAgentAppId;
        std::string mAgentAppKey;
        std::vector<uint8_t> mSeed;
        std::map<std::string, std::string> mDidProperties;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _ELASTOS_SDK_DID_IOTCLIENT_HPP_ */


