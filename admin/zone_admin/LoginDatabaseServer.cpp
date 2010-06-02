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

#include "LoginDatabaseServer.hpp"
using namespace cat;


//// LoginDatabaseConnexion

LoginDatabaseConnexion::LoginDatabaseConnexion()
{

}

LoginDatabaseConnexion::~LoginDatabaseConnexion()
{

}

void LoginDatabaseConnexion::OnConnect(ThreadPoolLocalStorage *tls)
{

}

void LoginDatabaseConnexion::OnDisconnect(u8 reason)
{

}

void LoginDatabaseConnexion::OnMessage(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes)
{

}

void LoginDatabaseConnexion::OnTick(ThreadPoolLocalStorage *tls, u32 now)
{

}


//// LoginDatabaseServer

LoginDatabaseServer::LoginDatabaseServer()
{

}

LoginDatabaseServer::~LoginDatabaseServer()
{

}

bool LoginDatabaseServer::Initialize()
{
	AddRef();

	Port port = (Port)Settings::ii->getInt("Zone.Server.Port", 22000);
	if (!port)
	{
		WARN("LoginDatabaseServer") << "Invalid port specified in settings file";
		return false;
	}

	ThreadPoolLocalStorage tls;
	if (!Initialize(&tls, port))
	{
		WARN("LoginDatabaseServer") << "Unable to start zone server";
		return false;
	}

	return true;
}

void LoginDatabaseServer::Shutdown()
{
	// Kill server
	Close();

	// And make sure it gets deleted
	ReleaseRef();
}

void LoginDatabaseServer::Pause()
{

}

void LoginDatabaseServer::Continue()
{

}

sphynx::Connexion *LoginDatabaseServer::NewConnexion()
{
	return new LoginDatabaseConnexion;
}

bool LoginDatabaseServer::AcceptNewConnexion(const NetAddr &src)
{
	return true; // allow all
}
