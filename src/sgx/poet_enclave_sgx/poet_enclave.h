/*
 Copyright 2018 Intel Corporation

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
------------------------------------------------------------------------------
*/

#pragma once

#include <string>
#include "poet.h"

// This is the identifier for the genesis block
const std::string NULL_IDENTIFIER = "0000000000000000";
const int IDENTIFIER_LENGTH = 16;
const double MINIMUM_WAIT_TIME = 1.0;

// Signup data is an ephemeral thing that is used in the creation of signup
// info.  When a request is made to create signup info, we first will create
// the signup data.  Part of the generated signup data (enclave quote)
// are used to create the IAS Attestation Verification Report (AVR).
// The signup data plus the AVR are then used to generate the signup info.
//
// Signup data is never meant to be used externally, thus the Python convention
// of prefixing the class name with an underscore.

class _SignupData
{
public:
    static _SignupData* CreateSignupData(
        const std::string& originatorPublicKeyHash
        );
    // Signup data properties
    std::string poet_public_key;
    std::string enclave_quote;

protected:
    _SignupData(
        const std::string& originatorPublicKeyHash
        );
}; // class _SignupData

_SignupData* _create_signup_data(
    const std::string& originator_public_key_hash
    );

void _destroy_signup_data(
    _SignupData* signup_data
    );

class SignupInfo
{
public:
    static SignupInfo* _SignupInfoFromSerialized(
        const std::string& serializedSignupInfo
        );

    std::string serialize() const
    {
        return this->serialized;
    }

    // Signup info properties
    std::string poet_public_key;
    std::string proof_data;
    std::string anti_sybil_id;

protected:
    SignupInfo(
        const std::string& serializedSignupInfo
        );

private:
    /*
    Json serialization of the signup info Parameters, this serves as the
    canonical representation of the signup info.
    */
    std::string serialized;
}; // class SignupInfo

SignupInfo* deserialize_signup_info(
    const std::string& serialized_signup_info
    );


class WaitCertificate
{
 public:
    static poet_err_t _InitializeWaitCertificate(
        const std::string& prevWaitCertificate,
        const std::string& validatorId,
        const std::string& prevWaitCertificateSig,
        const std::string& poetPubKey,
        uint8_t *duration,
        size_t durationLen
        );
    static WaitCertificate* _FinalizeWaitCertificate(
        const std::string& prevWaitCertificate,
        const std::string& prevBlockId,
        const std::string& prevWaitCertificateSig,
        const std::string& blockSummary,
        uint64_t waitTime
        );
    static WaitCertificate* _WaitCertificateFromSerialized(
        const std::string& serializedCertificate,
        const std::string& signature = ""
        );

    std::string identifier() const;
    std::string serialize() const;
    void deserialize(
        const std::string& serializeCertificate
        );

    // WaitCertificate Parameters
    std::string duration;
    std::string previous_block_id;
    std::string prev_wait_cert_sig;
    std::string validator_id;
    std::string block_summary;
    uint64_t block_num;
    uint64_t waitTime;
    /*
    Json serialization of the WaitCertificate Parameters, this serves as the
    canonical representation of the WaitCertificate. The signature field is
    used to verify that it has not been changed and that it was generated by a
    trusted source.
    */
    std::string serialized;
    /*
    Base64 encoded ECDSA signauture over the serialized WaitCerficate using the
    PoET secret key generated for the validator when its signup info was
    created.  The enclave can use this to verify that the wait timer generated
    by the associated validator's enclave and the serialized timer has not
    changed.
    */
    std::string signature;

protected:
    WaitCertificate(
        const std::string& prevWaitCertificate,
        const std::string& prevBlockId,
        const std::string& prevWaitCertificateSig,
        const std::string& blockSummary,
        uint64_t waitTime
        );
    WaitCertificate();
    /*WaitCertificate(
        const std::string& serializedCertificate,
        const std::string& signature = ""
        );*/
}; // class WaitCertificate


WaitCertificate* deserialize_wait_certificate(
    const std::string& serialized_certificate,
    const std::string& signature
    );

poet_err_t initialize_wait_certificate(
    const std::string& prevWaitCertificate,
    const std::string& validatorId,
    const std::string& prevWaitCertificateSig,
    const std::string& poetPubKey,
    uint8_t *duration,
    size_t durationLen
    );

WaitCertificate* finalize_wait_certificate(
    const std::string& prevWaitCertificate,
    const std::string& prevBlockId,
    const std::string& prevWaitCertificateSig,
    const std::string& blockSummary,
    uint64_t waitTime
    );

bool _verify_wait_certificate(
    const std::string& serializedWaitCertificate,
    const std::string& waitCertificateSignature,
    const std::string& poetPublicKey
    );

void _destroy_wait_certificate(WaitCertificate *waitCert);

class Poet
{
private:
    static Poet* instance;
    Poet(
        const std::string& enclaveModulePath,
        const std::string& spid
        );
public:
    static Poet *getInstance(
        const std::string& enclaveModulePath,
        const std::string& spid
        );
    // overriding copy constructor and assignment operator
    Poet(const Poet &) = delete;
    Poet & operator = (const Poet &) = delete;

    virtual ~Poet();
    std::string get_epid_group();
    void set_signature_revocation_list(
        const std::string& signature_revocation_list
        );

    std::string mr_enclave;         // hex encoding of the enclave measurement
    std::string basename;           // hex encoding of the basename
}; // class Poet

void InitializePoetEnclaveModule();
bool _is_sgx_simulator();
