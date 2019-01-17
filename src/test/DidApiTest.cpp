#include <iomanip>
#include <iostream>

#include "Elastos.SDK.DID.IOT.Api.hpp"
#include "common/Log.hpp"

static constexpr const char* TAG = "DidApiTest";

int main( int argc, char **argv )
{
    int ret = elastos::ElastosSdkDidIotApi::ErrCode::Unknown;

    std::string mnemonic;
    ret = elastos::ElastosSdkDidIotApi::GenerateMnemonic(mnemonic);
    if(ret < 0) {
        Log::E(TAG, "Failed to GenerateMnemonic ret = %d", ret);
    }
    Log::D(TAG, "GenerateMnemonic mnem = %s", mnemonic.c_str());

    elastos::ElastosSdkDidIotApi elastosDid;

    mnemonic = "supply lunch light powder page movie field diagram arm rabbit improve visa";
    ret = elastosDid.revertMnemonic(mnemonic);
    if(ret < 0) {
        Log::E(TAG, "Failed to revertMnemonic ret = %d", ret);
    }
    Log::D(TAG, "revertMnemonic mnem = %s", mnemonic.c_str());

    std::string did;
    elastosDid.getDeviceDid(did);
    Log::D(TAG, "ElastosDID = %s", did.c_str());

    using namespace std::chrono;
    milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    std::string key = "timestamp";
    std::string value = std::to_string(now.count());
    ret = elastosDid.setProperty(key, value);
    if(ret < 0) {
        Log::E(TAG, "Failed to setProperty ret = %d", ret);
    }
    Log::D(TAG, "setProperty %s: %s", key.c_str(), value.c_str());
    elastosDid.setProperty("dump", "-1");

    const char* defaultAgentURL = "https://api-wallet-did.elastos.org/api/1/blockagent/upchain/data";
    ret = elastosDid.setAgentUrl(defaultAgentURL);
    if(ret < 0) {
        Log::E(TAG, "Failed to setAgentUrl ret = %d", ret);
    }
    Log::D(TAG, "setAgentUrl %s", defaultAgentURL);

    const char* defaultAgentAppId = "org.elastos.debug.didagent";
    const char* defaultAgentAppKey = "b2gvzUM79yLhCbbGNWCuhSsGdqYhA7sS";
    ret = elastosDid.setAgentIdAndKey(defaultAgentAppId, defaultAgentAppKey);
    if(ret < 0) {
        Log::E(TAG, "Failed to setAgentIdAndKey ret = %d", ret);
    }
    Log::D(TAG, "setAgentIdAndKey id:%s, key:%s", defaultAgentAppId, defaultAgentAppId);

    ret = elastosDid.uploadToBlockchain();
    if(ret < 0) {
        Log::E(TAG, "Failed to uploadToBlockchain ret = %d", ret);
    }
    Log::D(TAG, "uploadToBlockchain success");

    return 0;
}
