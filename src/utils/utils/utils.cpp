#include "utils.h"

float Utils::kpa_to_psi(float kpa)
{
    return kpa / 6.895f;
}

float Utils::calculate_boost(int map, int ambient)
{
    float ambient_act = Utils::kpa_to_psi(ambient);
    float map_act = Utils::kpa_to_psi(map / 10.0f);

    float value = map_act - ambient_act;

    return value;
}