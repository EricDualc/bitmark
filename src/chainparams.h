// Copyright (c) 2009-2010 Satoshi Nakamoto
// Original Code: Copyright (c) 2009-2014 The Bitcoin Core Developers
// Modified Code: Copyright (c) 2014 Project Bitmark
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITMARK_CHAIN_PARAMS_H
#define BITMARK_CHAIN_PARAMS_H

#include "bignum.h"
#include "uint256.h"

#include <vector>

using namespace std;

#define MESSAGE_START_SIZE 4
typedef unsigned char MessageStartChars[MESSAGE_START_SIZE];

class CAddress;
class CBlock;

struct CDNSSeedData {
    string name, host;
    CDNSSeedData(const string &strName, const string &strHost) : name(strName), host(strHost) {}
};

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Bitmark system. There are three: the main network on which people trade goods
 * and services, the public test network which gets reset from time to time and
 * a regression test mode which is intended for private networks only. It has
 * minimal difficulty to ensure that blocks can be found instantly.
 */
class CChainParams
{
public:
    enum Network {
        MAIN,
        TESTNET,
        REGTEST,

        MAX_NETWORK_TYPES
    };

    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,

        MAX_BASE58_TYPES
    };

    const uint256& HashGenesisBlock() const { return hashGenesisBlock; }
    const MessageStartChars& MessageStart() const { return pchMessageStart; }
    const vector<unsigned char>& AlertKey() const { return vAlertPubKey; }
    int GetDefaultPort() const { return nDefaultPort; }
    const CBigNum& ProofOfWorkLimit() const { return bnProofOfWorkLimit; }
    int SubsidyHalvingInterval() const { return nSubsidyHalvingInterval; }
    int SubsidyInterimInterval() const { return nSubsidyHalvingInterval/2; }
    virtual const CBlock& GenesisBlock() const = 0;
    virtual bool RequireRPCPassword() const { return true; }
    const string& DataDir() const { return strDataDir; }
    virtual Network NetworkID() const = 0;
    const vector<CDNSSeedData>& DNSSeeds() const { return vSeeds; }
    const std::vector<unsigned char> &Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
    virtual const vector<CAddress>& FixedSeeds() const = 0;
    int RPCPort() const { return nRPCPort; }
    bool StrictChainId() const { return fStrictChainId; }
    int16_t GetAuxpowChainId() const { return nAuxpowChainId; }
    unsigned int EquihashN() const { return nEquihashN; }
    unsigned int EquihashK() const { return nEquihashK; }
    bool MineBlocksOnDemand() const { return fMineBlocksOnDemand; }

    int64_t GetFork2Height() const { return nForkHeight2; }
    bool    OnFork2(int64_t blockHeight) const { return blockHeight >= nForkHeight2; }

    // CEM Look-Back time frame (from which to find the refrence highest or peak hashrate)    Policy is relaxed to ~ 25% of original value.
    //       CEM v0.1 looks back 365 days.       CEM v0.2 looks back only 90 days
    //       This allow the maximum emission rate to be resumed that much sooner.
    int     CEM_WindowLength(int64_t blockHeight) const { return OnFork2(blockHeight) ? 90 : 365; }

    //  CEM is allowed to affect this portion of the epoch nominal block reward.
    //       CEM v0.1 scales 50% of the max epoch reward.     CEM v0.2 scales 80% of the max epoch reward.
    //         Both versions allow maximum theoretical emission rate if current hashrate is at peak performance,
    //         but CEM v0.2 has a stronger emission rate reduction if current hashrate is any less than reference peak performance.
    //
    int     CEM_MaxNativeBlockRewardReduction(int64_t blockHeight) const { return OnFork2(blockHeight) ? 80 : 50; }


protected:
    CChainParams() {}

    uint256 hashGenesisBlock;
    MessageStartChars pchMessageStart;
    // Raw pub key bytes for the broadcast alert signing key.
    vector<unsigned char> vAlertPubKey;
    int nDefaultPort;
    int nRPCPort;
    CBigNum bnProofOfWorkLimit;
    int nSubsidyHalvingInterval;
    string strDataDir;
    vector<CDNSSeedData> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    bool fStrictChainId;
    int16_t nAuxpowChainId;
    int nStartAuxPow;
    unsigned int nEquihashN = 0;
    unsigned int nEquihashK = 0;
    bool fMineBlocksOnDemand = true;

    int64_t nForkHeight2;
};

/**
 * Return the currently selected parameters. This won't change after app startup
 * outside of the unit tests.
 */
const CChainParams &Params();

/** Sets the params returned by Params() to those for the given network. */
void SelectParams(CChainParams::Network network);

/**
 * Looks for -regtest or -testnet and then calls SelectParams as appropriate.
 * Returns false if an invalid combination is given.
 */
bool SelectParamsFromCommandLine();

inline bool TestNet() {
    // Note: it's deliberate that this returns "false" for regression test mode.
    return Params().NetworkID() == CChainParams::TESTNET;
}

inline bool RegTest() {
    return Params().NetworkID() == CChainParams::REGTEST;
}

#endif
