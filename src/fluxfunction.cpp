/* 
 * Enough is enough! I have had it with all this spaghetti, all this copying, all this faxing!
 * Copyright (c) 2014 The FoxCoin Foxes
 * Copyright (c) 2014 The CoinDev Team
 */ 

#include "base58.h"
#include "fluxrpc.h"
#include "db.h"
#include "init.h"
#include "main.h"
#include "net.h"
#include "wallet.h"
#include "fluxfunction.h"
#include "util.h"

using namespace std;
using namespace boost;

double getRawDifficulty(const CBlockIndex* blockindex = NULL)
{
    if (blockindex == NULL)
    {
        if (pindexBest == NULL)
            return 1.0;
        else
            blockindex = pindexBest;
    }

    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

int GetRawNetworkHashesPS(int lookup)
{
    CBlockIndex *pb = pindexBest;
    
    if (pb == NULL || !pb->nHeight)
        return 0;

    // If lookup is -1, then use blocks since last difficulty change.
    if (lookup <= 0)
        lookup = pb->nHeight % 20 + 1;

    // If lookup is larger than chain, then set it to chain length.
    if (lookup > pb->nHeight)
        lookup = pb->nHeight;

    CBlockIndex *pb0 = pb;
    int64 minTime = pb0->GetBlockTime();
    int64 maxTime = minTime;
    for (int i = 0; i < lookup; i++) {
        pb0 = pb0->pprev;
        int64 time = pb0->GetBlockTime();
        minTime = std::min(time, minTime);
        maxTime = std::max(time, maxTime);
    }

    // In case there's a situation where minTime == maxTime, we don't want a divide by zero exception.
    if (minTime == maxTime)
        return 0;

    uint256 workDiff = pb->nChainWork - pb0->nChainWork;
    int64 timeDiff = maxTime - minTime;

    return (boost::int64_t)(workDiff.getdouble() / timeDiff);
}

double getTotalVolume()
{
    int nHeight = pindexBest->nHeight;
    int i = 0;
    double t = 0;
    while(i < nHeight+1)
    {
        t += getBlockReward(i);
        i++;
    }
    return t;
}

double getTotalVolumeN(int nHeight)
{
    int i = 0;
    double t = 0;
    while(i < nHeight+1)
    {
        t += getBlockReward(i);
        i++;
    }
    return t;
}

double getReward()
{
    int nHeight = pindexBest->nHeight;
    double nSubsidy = 1;
    
    if (nHeight == 0)
    {
        nSubsidy = 0;
    }

    if(nHeight < 5001)
    {
        nSubsidy = nHeight * 0.2; 
    } else if(nHeight < 10005001) {
        nSubsidy = (1000 - ((nHeight - 5001) * 0.0001));
    } else {
        nSubsidy = ((nHeight - 10005001) * 0.000001);
    }
    
    return nSubsidy;
}

double getBlockReward(int nHeight)
{
    double nSubsidy = 1;
    
    if (nHeight == 0)
    {
        nSubsidy = 0;
    }

    if(nHeight < 5001)
    {
        nSubsidy = nHeight * 0.2; 
    } else if(nHeight < 10005001) {
        nSubsidy = (1000 - ((nHeight - 5001) * 0.0001));
    } else {
        nSubsidy = ((nHeight - 10005001) * 0.000001);
    }
    
    return nSubsidy;
}

double GetRawEstimatedNextDifficulty(const CBlockIndex* blockindex = NULL){
   if (blockindex == NULL)
    {
        if (pindexBest == NULL)
            return 1.0;
        else
            blockindex = pindexBest;
    }

    unsigned int nBits;
    nBits = getnextworkrequiredlel(blockindex);

    int nShift = (nBits >> 24) & 0xff;

    double dDiff = (double)0x0000ffff / (double)(nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

double getBlockDifficulty(int height)
{
    const CBlockIndex* blockindex = getBlockIndex(height);
    
    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

double getRetarget(int height)
{
    return getBlockDifficulty(height) - getBlockDifficulty(height - 1);
}

int getBlockHashrate(int height)
{
    int lookup = height;
    
    double timeDiff = getBlockTimestamp(height) - getBlockTimestamp(1);
    double timePerBlock = timeDiff / lookup;

    return (boost::int64_t)(((double)getBlockDifficulty(height) * pow(2.0, 32)) / timePerBlock);
}

const CBlockIndex* getBlockIndex(int height)
{
    std::string hex = getBlockHash(height);
    uint256 hash(hex);
    return mapBlockIndex[hash];
}

int getBlockTime(int height)
{
    return getBlockIndex(height)->nTime - getBlockIndex(height - 1)->nTime;
}

std::string getBlockHash(int Height)
{
    if(Height > pindexBest->nHeight) { return "00000a1fb4bcc7a9d707c29f03d68f5d09d6bae41d4d05f682610b35a4556911"; }
    if(Height < 0) { return "00000a1fb4bcc7a9d707c29f03d68f5d09d6bae41d4d05f682610b35a4556911"; }
    int desiredheight;
    desiredheight = Height;
    if (desiredheight < 0 || desiredheight > nBestHeight)
        return 0;
        
    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hashBestChain];
    while (pblockindex->nHeight > desiredheight)
        pblockindex = pblockindex->pprev;
    return pblockindex->phashBlock->GetHex();
}

int getBlockTimestamp(int Height)
{
    std::string strHash = getBlockHash(Height);
    uint256 hash(strHash);

    if (mapBlockIndex.count(hash) == 0)
        return 0;

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    return pblockindex->nTime;
}

std::string getBlockMerkle(int Height)
{
    std::string strHash = getBlockHash(Height);
    uint256 hash(strHash);
    
    if (mapBlockIndex.count(hash) == 0)
        return 0;

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    return pblockindex->hashMerkleRoot.ToString().substr(0,10).c_str();
}

int getBlocknBits(int Height)
{
    std::string strHash = getBlockHash(Height);
    uint256 hash(strHash);
    
    if (mapBlockIndex.count(hash) == 0)
        return 0;

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    return pblockindex->nBits;
}

int getBlockNonce(int Height)
{
    std::string strHash = getBlockHash(Height);
    uint256 hash(strHash);

    if (mapBlockIndex.count(hash) == 0)
        return 0;

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    return pblockindex->nNonce;
}

std::string getBlockDebug(int Height)
{
    std::string strHash = getBlockHash(Height);
    uint256 hash(strHash);

    if (mapBlockIndex.count(hash) == 0)
        return 0;

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    return pblockindex->ToString();
}

int blocksInPastHours(int hours)
{
    int wayback = hours * 3600;
    bool check = true;
    int height = pindexBest->nHeight;
    int heightHour = pindexBest->nHeight;
    int utime = (int)time(NULL);
    int target = utime - wayback;
    
    while(check)
    {
        if(heightHour == 1)
        {
            return height - heightHour;
        }
        if(getBlockTimestamp(heightHour) < target)
        {
            check = false;
            return height - heightHour;
        } else {
            heightHour = heightHour - 1;
        }
    }
}

double getTxTotalValue(std::string txid)
{
    uint256 hash;
    hash.SetHex(txid);

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true))
        return 51;
    
    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;

    double value = 0;
    double buffer = 0;
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        const CTxOut& txout = tx.vout[i];
 
        buffer = value + convertFlux(txout.nValue);
        value = buffer;
    }

    return value;
}

double convertFlux(int64 amount)
{
    return (double)amount / (double)FLUX;
}

std::string getOutputs(std::string txid)
{
    uint256 hash;
    hash.SetHex(txid);

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true))
        return "fail";
    
    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;

    std::string str = "";
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        const CTxOut& txout = tx.vout[i];
        CTxDestination source;
        ExtractDestination(txout.scriptPubKey, source);  
        CFluxAddress addressSource(source); 
        std::string lol7 = addressSource.ToString();
        double buffer = convertFlux(txout.nValue);
        std::string amount = boost::to_string(buffer);
        str.append(lol7);
        str.append(": ");
        str.append(amount);
        str.append(" FLX");
        str.append("\n");        
    }

    return str;
}

std::string getInputs(std::string txid)
{
    uint256 hash;
    hash.SetHex(txid);

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true))
        return "fail";
    
    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;

    std::string str = "";
    for (unsigned int i = 0; i < tx.vin.size(); i++)
    {     
        uint256 hash;
        const CTxIn& vin = tx.vin[i];
        hash.SetHex(vin.prevout.hash.ToString());
        CTransaction wtxPrev;                                     
        uint256 hashBlock = 0;
        if (!GetTransaction(hash, wtxPrev, hashBlock))
             return "fail";
    
        CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
        ssTx << wtxPrev;

        CTxDestination source;
        ExtractDestination(wtxPrev.vout[vin.prevout.n].scriptPubKey, source);  
        CFluxAddress addressSource(source); 
        std::string lol6 = addressSource.ToString();
        const CScript target = wtxPrev.vout[vin.prevout.n].scriptPubKey;
        double buffer = convertFlux(getInputValue(wtxPrev, target));
        std::string amount = boost::to_string(buffer);
        str.append(lol6);
        str.append(": ");
        str.append(amount);
        str.append(" FLX");
        str.append("\n");        
    }

    return str;  
}

int64 getInputValue(CTransaction tx, CScript target)
{
    for (unsigned int i = 0; i < tx.vin.size(); i++)
    {
        const CTxOut& txout = tx.vout[i];
        if(txout.scriptPubKey == target)
        {
            return txout.nValue;
        }
    }
}

double getTxFees(std::string txid)
{
    uint256 hash;
    hash.SetHex(txid);

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true))
        return 51;
    
    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;

    double value = 0;
    double buffer = 0;
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        const CTxOut& txout = tx.vout[i];
 
        buffer = value + convertFlux(txout.nValue);
        value = buffer;
    }
    
    double value0 = 0;
    double buffer0 = 0;
    for (unsigned int i = 0; i < tx.vin.size(); i++)
    {     
        uint256 hash0;
        const CTxIn& vin = tx.vin[i];
        hash0.SetHex(vin.prevout.hash.ToString());
        CTransaction wtxPrev;  
        uint256 hashBlock0 = 0;
        if (!GetTransaction(hash0, wtxPrev, hashBlock0))
             return 0;
        CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
        ssTx << wtxPrev;
        const CScript target = wtxPrev.vout[vin.prevout.n].scriptPubKey;
        buffer0 = value0 + convertFlux(getInputValue(wtxPrev, target));
        value0 = buffer0;
    }

    return value0 - value;
}

std::string getNodeInfo()
{
    LOCK(cs_vNodes);
    int i = vNodes.size();
    std::string str;
    while(i > 0)
    {
        i--;
        CNodeStats stats;
        CNode* pnode = vNodes[i];
        str.append(pnode->addrName);
        str.append(" [");    
        str.append(pnode->strSubVer);
        str.append("]");
        str.append("\n");
    }
    return str;
}

bool addnode(std::string node)
{
    string strNode = node;
    strNode.append(":7970");
    CAddress addr;
    bool exists = false;
    LOCK(cs_vNodes);
    int i = vNodes.size();
    while(i > 0)
    {
        i--;
        CNodeStats stats;
        CNode* pnode = vNodes[i];
        if(pnode->addrName == strNode)
        {
            exists = true;
            return false;
        }
    }
    if(!exists)
    {
    ConnectNode(addr, strNode.c_str());    
    int i1 = vNodes.size();
    while(i1 > 0)
    {
        i1--;
        CNodeStats stats;
        CNode* pnode = vNodes[i1];
        if(pnode->addrName == strNode)
        {
            return true;
        }
    }
        return false;
    }
}

bool isFluxbase(std::string txid)
{
    uint256 hash;
    hash.SetHex(txid);

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true))
        return 51;
    
    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;
    
    return tx.IsFluxBase();
}

double GetEstimatedNextDifficulty()
{
    return GetRawEstimatedNextDifficulty();
}

double getDifficulty()
{
    return getRawDifficulty();
}

int getNetworkHashesPS()
{
    return GetRawNetworkHashesPS(-1);
}

