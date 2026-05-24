#include "can_helper.h"

twai_message_t CanHelper::create_can_request(int uds_id)
{
    twai_message_t msg = {};

    int can_id = CANId::REQUEST;
    int service_id = get_service_id(uds_id);
    int hi_id = (uds_id & 0xFF00) >> 8;
    int lo_id = uds_id & 0x00FF;
    // always three for now
    int payload_length = 3;

    msg.identifier = can_id;
    msg.data_length_code = 8;
    msg.extd = 0;

    msg.data[0] = payload_length;
    msg.data[1] = service_id;
    msg.data[2] = hi_id;
    msg.data[3] = lo_id;

    return msg;
};

int CanHelper::get_service_id(int uds_id)
{
    switch (uds_id)
    {
    case UDSId::ACCELERATOR_POS:
    case UDSId::AMBIENT_AIR:
    case UDSId::MANIFOLD_AIR:
        return ServiceId::SERVICE_REQUEST;
    default:
        return -1;
    }
}