#include "global.h"
#include "NetworkProtocol.h"
#include "NetworkProtocolLegacy.h"
#include "ezsockets.h"
#include "NetworkSyncManager.h"
#include "NetworkPacket.h"

NetworkProtocolLegacy::NetworkProtocolLegacy()
{
	m_sName = "Legacy";
}

NetworkProtocolLegacy::~NetworkProtocolLegacy()
{
}

void NetworkProtocolLegacy::ParseInput(NetworkPacket *p)
{
}

// Server Packet Handlers

// Client Packet Handlers
void NetworkProtocolLegacy::Hello( const RString sClientName )
{
	m_Packet.Clear();
	m_Packet.Write1(LegacyCmd_Hello);
	m_Packet.Write1(LegacyProtocolVersion);
	m_Packet.WriteString( sClientName );
}

void NetworkProtocolLegacy::SendLogin(uint8_t iPlayer, uint8_t iAuthMethod, RString sPlayerName, RString sHashedName)
{
	// broken
	/*
	NSMAN->m_SMOnlinePacket.Clear();
	NSMAN->m_SMOnlinePacket.Write1(0); // login
	NSMAN->m_SMOnlinePacket.Write1( iPlayer ); // Player
	NSMAN->m_SMOnlinePacket.Write1( iAuthMethod ); // Auth method
	NSMAN->m_SMOnlinePacket.WriteString( sPlayerName );
	NSMAN->m_SMOnlinePacket.WriteString( sHashedName );
	SendSMOnline(NSMAN->&m_SMOnlinePacket);
	*/
}

void NetworkProtocolLegacy::SendSMOnline(NetworkPacket *p)
{
	p->Position = NSMAN->m_SMOnlinePacket.Position + 1;
	memcpy( (p->Data + 1), NSMAN->m_SMOnlinePacket.Data, NSMAN->m_SMOnlinePacket.Position );
	p->Data[0] = LegacyCmd_SMOnline;
	NSMAN->SendPacket(p);
}

/*
 * (c) 2011 AJ Kelly
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */