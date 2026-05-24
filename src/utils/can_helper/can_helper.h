#pragma once

#include "driver/twai.h"
#include "utils/uds_id.h"
#include "utils/can_id.h"
#include "utils/service_id.h"

class CanHelper
{
    public:
        static twai_message_t create_can_request(int uds_id);
    private:
        static int get_service_id(int uds_id);
};