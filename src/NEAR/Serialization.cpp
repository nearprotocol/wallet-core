// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Serialization.h"

#include "../NEAR/Address.h"
#include "../Base64.h"
#include <TrustWalletCore/TWHRP.h>

using namespace TW;
using namespace TW::NEAR;
using namespace TW::NEAR::Proto;


static void writeU8(Data& data, uint8_t number) {
    data.push_back(number);
}

static void writeU32(Data& data, uint32_t number) {
    data.push_back(number & 0xFF);
    data.push_back((number >> 8) & 0xFF);
    data.push_back((number >> 16) & 0xFF);
    data.push_back((number >> 24) & 0xFF);
}

static void writeU64(Data& data, uint64_t number) {
    data.push_back(number & 0xFF);
    data.push_back((number >> 8) & 0xFF);
    data.push_back((number >> 16) & 0xFF);
    data.push_back((number >> 24) & 0xFF);
    data.push_back((number >> 32) & 0xFF);
    data.push_back((number >> 40) & 0xFF);
    data.push_back((number >> 48) & 0xFF);
    data.push_back((number >> 56) & 0xFF);
}

static void writeU128(Data& data, const Proto::Uint128 number) {
    const auto& numberData = number.number();
    data.insert(std::end(data), std::begin(numberData), std::end(numberData));
}

static void writeString(Data& data, const std::string& str) {
    writeU32(data, str.length());
    data.insert(std::end(data), std::begin(str), std::end(str));
}

static void writePublicKey(Data& data, const Proto::PublicKey& publicKey) {
    writeU8(data, publicKey.key_type());
    const auto& keyData = publicKey.data();
    data.insert(std::end(data), std::begin(keyData), std::end(keyData));
}

static void writeTransfer(Data& data, const Proto::Transfer& transfer) {
    writeU128(data, transfer.deposit());
}

static void writeAction(Data& data, const Proto::Action& action) {
    writeU8(data, action.payload_case() - Proto::Action::kCreateAccount);
    switch (action.payload_case()) {
        case Proto::Action::kTransfer:
            writeTransfer(data, action.transfer());
            return;
        default:
            // TODO: Report error properly?
            return;
    }
}

Data TW::NEAR::transactionData(const Proto::SigningInput& input) {
    Data data;
    writeString(data, input.signer_id());
    writePublicKey(data, input.public_key());
    writeU64(data, input.nonce());
    writeString(data, input.receiver_id());
    // TODO: assert data sizes
    const auto& block_hash = input.block_hash();
    data.insert(std::end(data), std::begin(block_hash), std::end(block_hash));
    writeU32(data, input.actions_size());
    for (const auto& action : input.actions()) {
        writeAction(data, action);
    }
    return data;
}

Data TW::NEAR::signedTransactionData(const Data& transactionData, const Data& signatureData) {
    return Data();
}
