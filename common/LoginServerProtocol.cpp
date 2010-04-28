/*
	Copyright (c) 2009-2010 Christopher A. Taylor.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of LibCat nor the names of its contributors may be used
	  to endorse or promote products derived from this software without
	  specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "LoginServerProtocol.hpp"
#include <cctype>
using namespace cat;

bool cat::ValidLoginServerUserName(const void *vname, int bytes)
{
	if (bytes < USER_NAME_MINLEN || bytes > USER_NAME_MAXLEN) return false;

	const char *name = reinterpret_cast<const char*>( vname );

	// First character:
	char prev = name[0];
	if (!std::isalpha(prev)) return false;

	// For each remaining character,
	while (--bytes)
	{
		char next = *++name;
		if (!std::isalnum(next)) return false;
		//prev = next;
	}

	// Made it through all the filters successfully
	return true;
}

void cat::GeneratePasswordHash(const char *username, const void *salt, int salt_bytes, const char *password, void *password_hash, int password_hash_bytes)
{
	Skein hash;

	// Sanitize the username
	char sane_name[USER_NAME_MAXLEN+1];
	CopyDesimilarizeString(username, sane_name);

	hash.BeginKey(256);
	hash.CrunchString(sane_name);
	hash.CrunchString("\rSplaner\n");
	hash.Crunch(salt, salt_bytes);
	hash.CrunchString(password);
	hash.End();

	hash.Generate(password_hash, password_hash_bytes, PASSWORD_STRENGTHENING_ROUNDS);
}

void cat::GeneratePasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes,
								const void *server_nonce, int server_nonce_bytes, void *proof_hash, int proof_hash_bytes)
{
	Skein key;

	key.BeginKey(256);
	key.Crunch(password_hash, password_hash_bytes);
	key.End();

	Skein hash(key);

	hash.BeginMAC();
	hash.Crunch(server_nonce, server_nonce_bytes);
	hash.Crunch(client_nonce, client_nonce_bytes);
	hash.End();

	hash.Generate(proof_hash, proof_hash_bytes);
}

bool cat::ValidPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes,
							 const void *server_nonce, int server_nonce_bytes, const void *proof_hash, int proof_hash_bytes)
{
	u8 *expected = (u8*)alloca(password_hash_bytes);

	Skein key;

	key.BeginKey(256);
	key.Crunch(password_hash, password_hash_bytes);
	key.End();

	Skein hash(key);

	hash.BeginMAC();
	hash.Crunch(server_nonce, server_nonce_bytes);
	hash.Crunch(client_nonce, client_nonce_bytes);
	hash.End();

	hash.Generate(expected, proof_hash_bytes);

	return SecureEqual(expected, proof_hash, proof_hash_bytes);
}

void cat::GenerateServerPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes, void *proof_hash, int proof_hash_bytes)
{
	Skein key;

	key.BeginKey(256);
	key.Crunch(password_hash, password_hash_bytes);
	key.End();

	Skein hash(key);

	hash.BeginMAC();
	hash.Crunch(client_nonce, client_nonce_bytes);
	hash.End();

	hash.Generate(proof_hash, proof_hash_bytes);
}

bool cat::ValidServerPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes, const void *proof_hash, int proof_hash_bytes)
{
	u8 *expected = (u8*)alloca(proof_hash_bytes);

	Skein key;

	key.BeginKey(256);
	key.Crunch(password_hash, password_hash_bytes);
	key.End();

	Skein hash(key);

	hash.BeginMAC();
	hash.Crunch(client_nonce, client_nonce_bytes);
	hash.End();

	hash.Generate(expected, proof_hash_bytes);

	return SecureEqual(expected, proof_hash, proof_hash_bytes);
}

bool cat::GenerateKeyProof(ThreadPoolLocalStorage *tls, const u8 *public_key, int public_key_bytes, const u8 *private_key, int private_key_bytes,
						   const void *client_nonce, int client_nonce_bytes, const void *server_nonce, int server_nonce_bytes, u8 *signature, int signature_bytes)
{
	u8 *plaintext = (u8*)alloca(client_nonce_bytes + server_nonce_bytes);

	KeyAgreementResponder responder;

	memcpy(plaintext, client_nonce, client_nonce_bytes);
	memcpy(plaintext + client_nonce_bytes, server_nonce, server_nonce_bytes);

	return responder.Initialize(tls->math, tls->csprng, public_key, public_key_bytes, private_key, private_key_bytes) &&
		   responder.Sign(tls->math, tls->csprng, plaintext, sizeof(plaintext), signature, signature_bytes);
}

bool cat::ValidateKeyProof(ThreadPoolLocalStorage *tls, const u8 *public_key, int public_key_bytes, const void *client_nonce, int client_nonce_bytes,
						const void *server_nonce, int server_nonce_bytes, const u8 *signature, int signature_bytes)
{
	u8 *plaintext = (u8*)alloca(client_nonce_bytes + server_nonce_bytes);

	KeyAgreementInitiator initiator;

	memcpy(plaintext, client_nonce, client_nonce_bytes);
	memcpy(plaintext + client_nonce_bytes, server_nonce, server_nonce_bytes);

	return initiator.Initialize(tls->math, public_key, public_key_bytes) &&
		   initiator.Verify(tls->math, plaintext, sizeof(plaintext), signature, signature_bytes);
}
