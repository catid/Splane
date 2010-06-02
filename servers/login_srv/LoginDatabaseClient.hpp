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

#ifndef CAT_LOGIN_DATABASE_CLIENT_HPP
#define CAT_LOGIN_DATABASE_CLIENT_HPP

#include <cat/AllFramework.hpp>

namespace cat {


class LoginServer;


typedef fastdelegate::FastDelegate4<ThreadPoolLocalStorage *, bool, BufferStream , u32> TransactionCallback;

class LoginTransaction
{
	friend class LoginDatabaseClient;

	LoginDatabaseClient *_client;		// Client object with a reference held
	u32 _id;							// Identifier for this transaction
	LoginTransaction *_next, *_prev;	// Linked list neighbors
	TransactionCallback _callback;		// Callback for completion success or failure

public:
	LoginTransaction();					// Ctor
	~LoginTransaction();				// Dtor: Releases held reference and unlinks

	void Release();						// Release held reference and unlink self
										// Does not invoke callback
};


enum LDC_DisconnectReasons
{
	LDC_DISCO_WRONG_KEY,		// Database Server rejected our access key!
	LDC_DISCO_BAD_SIGNATURE,	// Database Server rejected our signature!
	LDC_DISCO_CANNOT_SIGN,		// Couldn't generate signature
};


class LoginDatabaseClient : public sphynx::Client
{
	LoginServer *_owner;

	enum SessionState
	{
		CS_CHALLENGE,
		CS_LOGIN_WAIT,
		CS_READY
	} _state;

	Mutex _transaction_lock;
	LoginTransaction *_transaction_head, *_transaction_tail;
	u32 _next_transaction_id;

public:
	LoginDatabaseClient(LoginServer *owner);
	virtual ~LoginDatabaseClient();

	void ConnectToDatabase(ThreadPoolLocalStorage *tls);

	CAT_INLINE bool IsReady() { return _state == CS_READY; }

public:
	// Request should have DB_OVERHEAD_BYTES of space at the start, bytes includes this overhead
	bool Query(LoginTransaction *transaction, u8 *request, int bytes, const TransactionCallback &);
	void ForgetQuery(LoginTransaction *transaction);

protected:
	void OnClose();
	void OnConnectFail(sphynx::HandshakeError err);
	void OnConnect(ThreadPoolLocalStorage *tls);
	void OnDisconnect(u8 reason);
	void OnTimestampDeltaUpdate(u32 rtt, s32 delta);
	void OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnTick(ThreadPoolLocalStorage *tls, u32 now);

	bool OnChallenge(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes);

	void AnswerQuery(ThreadPoolLocalStorage *tls, u32 id, BufferStream request, int bytes);
};


} // namespace cat

#endif // CAT_LOGIN_DATABASE_CLIENT_HPP
