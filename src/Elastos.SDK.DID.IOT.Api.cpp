#include <Elastos.SDK.DID.IOT.Api.hpp>

#include <iomanip>
#include <sstream>

#include <openssl/md5.h>
#include <Log.hpp>
#include <Elastos.Wallet.Utility.h>
#include <wrapper/httpclient/HttpClient.hpp>

namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */

/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int ElastosSdkDidIotApi::GenerateMnemonic(std::string& mnemonic)
{
    char* newMnem = generateMnemonic(MnemonicLanguage, "");
    if(newMnem == nullptr) {
        Log::E(Log::TAG, "%s Failed to call generateMnemonic()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }

    mnemonic = std::string(newMnem);
    freeBuf(newMnem);

    return 0;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
ElastosSdkDidIotApi::ElastosSdkDidIotApi()
    : mAgentUrl(DefaultAgentURL)
    , mAgentAppId()
    , mAgentAppKey()
    , mSeed()
    , mDidProperties()
{
}

ElastosSdkDidIotApi::~ElastosSdkDidIotApi()
{
}

int ElastosSdkDidIotApi::revertMnemonic(const std::string& mnemonic)
{
    size_t count = std::count(mnemonic.begin(), mnemonic.end(), ' ');
    if(count != 11) {
        Log::E(Log::TAG, "%s Argument %s is not valid.", __PRETTY_FUNCTION__, mnemonic.c_str());
        return ErrCode::BadArgument;
    }

    uint8_t* seed = nullptr;
    int seedLen = getSeedFromMnemonic(reinterpret_cast<void**>(&seed), mnemonic.c_str(), MnemonicLanguage, "", "");
    if(seedLen <= 0) {
        Log::E(Log::TAG, "%s Failed to call getSeedFromMnemonic()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }

    mSeed.assign(seed, seed + seedLen);
    freeBuf(seed);

    return 0;
}

int ElastosSdkDidIotApi::setProperty(const std::string& key, const std::string& value)
{
    if(key.empty() == true) {
        Log::E(Log::TAG, "%s Empty argument key is not valid.", __PRETTY_FUNCTION__);
        return ErrCode::BadArgument;
    }

    mDidProperties[key] = value;
    Log::D(Log::TAG, "%s %s: %s", __PRETTY_FUNCTION__, key.c_str(), value.c_str());

    return 0;
}

int ElastosSdkDidIotApi::uploadToBlockchain()
{
    int ret = ErrCode::Unknown;

    std::string serialProps;
    ret = getSerialProperties(mDidProperties, serialProps);
    if(ret < 0) {
        return ret;
    }

    std::string signedProps;
    ret = getSignedProperties(mSeed, serialProps, signedProps);
    if(ret < 0) {
        return ret;
    }

    std::string formatAgentProps;
    ret = formatAgentProperties(mSeed, serialProps, signedProps, formatAgentProps);
    if(ret < 0) {
        return ret;
    }

    std::string authValue;
    ret = getAgentAuthValue(authValue);
    if(ret < 0) {
        return ret;
    }

    elastos::HttpClient::InitGlobal();

    elastos::HttpClient httpClient;
    std::string& body = formatAgentProps;

    ret = httpClient.Url(mAgentUrl);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }

    httpClient.SetTimeout(10000);
    httpClient.SetHeader("Content-Type", "application/json;charset=utf-8");
    httpClient.SetHeader(AgentAuthHeaderName, authValue);

    Log::V(Log::TAG, "%s post body: %s", __PRETTY_FUNCTION__, body.c_str());
    ret = httpClient.SyncPost(body);
    if(ret < 0) {
        ret = ErrCode::HttpClientError + ret;
        Log::E(Log::TAG, "%s Failed to post props to: %s. ret=%d", __PRETTY_FUNCTION__, mAgentUrl.c_str(), ret);
        return ret;
    }

    ret = httpClient.GetResponseStatus();
    if(ret < 0) {
        ret = ErrCode::HttpClientError + ret;
        Log::E(Log::TAG, "%s Failed to get response status from: %s. ret=%d", __PRETTY_FUNCTION__, mAgentUrl.c_str(), ret);
        return ret;
    }

    ret = httpClient.GetResponseBody(body);
    if(ret < 0) {
        ret = ErrCode::HttpClientError + ret;
        Log::E(Log::TAG, "%s Failed to get response body from: %s. ret=%d", __PRETTY_FUNCTION__, mAgentUrl.c_str(), ret);
        return ret;
    }
    Log::V(Log::TAG, "%s response body: %s", __PRETTY_FUNCTION__, body.c_str());

    return ret;
}

int ElastosSdkDidIotApi::setAgentUrl(const std::string& agentUrl)
{
    if(agentUrl.empty() == true) {
        Log::E(Log::TAG, "%s Empty argument not allowed.", __PRETTY_FUNCTION__);
        return ErrCode::NullArgument;
    }

    if(agentUrl.rfind(SchemeHttp) != 0 // start with
    && agentUrl.rfind(SchemeHttps) != 0) {
        Log::E(Log::TAG, "%s Argument %s is not supported.", __PRETTY_FUNCTION__, agentUrl.c_str());
        return ErrCode::BadArgument;
    }

    mAgentUrl = agentUrl;
    return 0;
}

int ElastosSdkDidIotApi::setAgentIdAndKey(const std::string& appId, const std::string& appKey)
{
    if(appId.empty() == true || appKey.empty() == true) {
        Log::E(Log::TAG, "%s Agent app id or key is not valid.", __PRETTY_FUNCTION__);
        return ErrCode::BadArgument;
    }

    mAgentAppId = appId;
    mAgentAppKey = appKey;

    return 0;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
int ElastosSdkDidIotApi::getDeviceDid(std::string& did) const
{
    if(mSeed.size() <= 0) {
        Log::E(Log::TAG, "%s Mnemonic is not revert.", __PRETTY_FUNCTION__);
        return ErrCode::MnemonicNotExists;
    }

    char* ret = getSinglePublicKey(mSeed.data(), mSeed.size());
    if(ret == nullptr) {
        Log::E(Log::TAG, "%s Failed to call getSinglePublicKey()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }
    std::string pubKey(ret);
    freeBuf(ret);

    ret = getDid(pubKey.c_str());
    if(ret == nullptr) {
        Log::E(Log::TAG, "%s Failed to call getDid()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }
    did = std::string(ret);
    freeBuf(ret);

    Log::D(Log::TAG, "%s devDid=%s", __PRETTY_FUNCTION__, did.c_str());

    return 0;
}

int ElastosSdkDidIotApi::getSerialProperties(const std::map<std::string, std::string>& props,
                                             std::string& serialProps) const
{
    std::stringstream sstream;
    sstream << "{";
    sstream << "  \"Tag\": \"DID Property\",";
    sstream << "  \"Ver\": \"1.0\",";
    sstream << "  \"Status\": \"Normal\",";
    sstream << "  \"Properties\": [";
    int idx = 0;
    for (auto& [key, val]: props) {
        if(idx++ > 0) {
            sstream << ",";
        }

        sstream << "    {";
        sstream << "      \"Key\": \"" << key << "\",";
        sstream << "      \"Value\": \"" << val << "\",";
        sstream << "      \"Status\": \"Normal\"";
        sstream << "    }";
    }
    sstream << "  ]";
    sstream << "}";

    serialProps = sstream.str();
    Log::D(Log::TAG, "%s serialProps=%s", __PRETTY_FUNCTION__, serialProps.c_str());

    return 0;
}

int ElastosSdkDidIotApi::getSignedProperties(const std::vector<uint8_t>& seed,
                                             const std::string& serialProps,
                                             std::string& signedProps) const
{
    if(mSeed.size() <= 0) {
        Log::E(Log::TAG, "%s Mnemonic is not revert.", __PRETTY_FUNCTION__);
        return ErrCode::MnemonicNotExists;
    }

    if(serialProps.empty() == true) {
        Log::E(Log::TAG, "%s Empty argument not allowed.", __PRETTY_FUNCTION__);
        return ErrCode::NullArgument;
    }

    char* ret = getSinglePrivateKey(seed.data(), seed.size());
    if(ret == nullptr) {
        Log::E(Log::TAG, "%s Failed to call getSinglePrivateKey()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }
    std::string privKey(ret);
    freeBuf(ret);

    void* signedData = nullptr;
    int signedLen = sign(privKey.c_str(), serialProps.data(), serialProps.size(), &signedData);
    if(signedLen <= 0 || signedData == nullptr) {
        Log::E(Log::TAG, "%s Failed to call sign()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }

    const uint8_t* signedPtr = reinterpret_cast<const uint8_t*>(signedData);
    std::stringstream sstream;
    for(int idx = 0; idx < signedLen; idx++) {
        sstream << std::hex << std::setw(2) << std::setfill('0') << (int)signedPtr[idx];
    }
    std::string hexSerialProps = sstream.str();
    Log::V(Log::TAG, "%s hexSerialProps: %s", __PRETTY_FUNCTION__, hexSerialProps.c_str());

    signedProps = sstream.str();
    freeBuf(signedData);

    return 0;
}

int ElastosSdkDidIotApi::formatAgentProperties(const std::vector<uint8_t>& seed,
                                               const std::string& serialProps,
                                               const std::string& signedProps,
                                               std::string& formatProps) const
{
    if(mSeed.size() <= 0) {
        Log::E(Log::TAG, "%s Mnemonic is not revert.", __PRETTY_FUNCTION__);
        return ErrCode::MnemonicNotExists;
    }

    char* ret = getSinglePublicKey(seed.data(), seed.size());
    if(ret == nullptr) {
        Log::E(Log::TAG, "%s Failed to call getSinglePublicKey()", __PRETTY_FUNCTION__);
        return ErrCode::WalletLibError;
    }
    std::string pubKey(ret);
    freeBuf(ret);
    Log::V(Log::TAG, "%s pubKey: %s", __PRETTY_FUNCTION__, pubKey.c_str());

    std::stringstream sstream;

    // convert string to hex
    const uint8_t* serialPtr = reinterpret_cast<const uint8_t*>(serialProps.c_str());
    sstream.str(std::string());
    sstream << std::setw(2) << std::setfill('0') << std::hex;
    for(int idx = 0; idx < serialProps.length(); idx++) {
        sstream << std::hex << std::setw(2) << std::setfill('0') << (int)serialPtr[idx];
    }
    std::string hexSerialProps = sstream.str();
    Log::V(Log::TAG, "%s hexSerialProps: %s", __PRETTY_FUNCTION__, hexSerialProps.c_str());

    Log::V(Log::TAG, "%s signedProps: %s", __PRETTY_FUNCTION__, signedProps.c_str());

    sstream.str(std::string());
    sstream << "{";
    sstream << "  \"pub\": \"" << pubKey << "\",";
    sstream << "  \"msg\": \"" << hexSerialProps << "\",";
    sstream << "  \"sig\": \"" << signedProps << "\"";
    sstream << "}";

    formatProps = sstream.str();

    return 0;
}

int ElastosSdkDidIotApi::getAgentAuthValue(std::string& authValue) const
{
    if(mAgentAppId.empty() == true || mAgentAppKey.empty() == true) {
        Log::E(Log::TAG, "%s Agent app id or key is not valid.", __PRETTY_FUNCTION__);
        return ErrCode::MnemonicNotExists;
    }

    using namespace std::chrono;
    milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    //std::string time = (std::stringstream() << now.count()).str();
    std::string time = std::to_string(now.count());

    std::string brief = mAgentAppKey + time;
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(brief.c_str()), brief.length(), md5);
    std::string auth(reinterpret_cast<char*>(md5));

    authValue = "id:" + mAgentAppId + ";time=" + time + ";auth=" + auth;
    return 0;
}

} // namespace elastos
