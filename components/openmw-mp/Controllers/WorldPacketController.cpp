#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include "../Packets/World/PacketObjectRemoval.hpp"

#include "WorldPacketController.hpp"

template <typename T>
inline void AddPacket(mwmp::WorldPacketController::packets_t *packets, RakNet::RakPeerInterface *peer)
{
    T *packet = new T(peer);
    typedef mwmp::WorldPacketController::packets_t::value_type value_t;
    packets->insert(value_t(packet->GetPacketID(), value_t::second_type(packet)));
}

mwmp::WorldPacketController::WorldPacketController(RakNet::RakPeerInterface *peer)
{
    AddPacket<PacketObjectRemoval>(&packets, peer);
}


mwmp::WorldPacket *mwmp::WorldPacketController::GetPacket(RakNet::MessageID id)
{
    return packets[(unsigned char)id].get();
}

void mwmp::WorldPacketController::SetStream(RakNet::BitStream *inStream, RakNet::BitStream *outStream)
{
    BOOST_FOREACH( packets_t::value_type &packet, packets )
        packet.second->SetStreams(inStream, outStream);
}