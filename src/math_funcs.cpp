#include <gensokyo.hpp>
#include <random>

float gensokyo::math::rad_to_deg(const float rad)
{
    return rad * (180.f / std::numbers::pi_v<float>);
}

float gensokyo::math::deg_to_rad(const float deg)
{
    return deg * (std::numbers::pi_v<float> / 180.f);
}

float gensokyo::math::normalize_yaw(const float yaw, const float range)
{
    return std::isfinite(yaw) ? std::remainderf(yaw, range) : 0.f;
}

void gensokyo::math::sin_cos(const float rad, float& sine, float& cosine)
{
    sine   = std::sin(rad);
    cosine = std::cos(rad);
}

void gensokyo::math::transform(const fVector3& tr, Matrix3x4& matrix, fVector3& out)
{
    out.x = tr.dot_product(matrix.at(0)) + matrix.at(0, 3);
    out.y = tr.dot_product(matrix.at(1)) + matrix.at(1, 3);
    out.z = tr.dot_product(matrix.at(2)) + matrix.at(2, 3);
}

float gensokyo::math::random_float(const float min, const float max)
{
    static std::random_device rd;
    static std::mt19937 e2(rd());
    std::uniform_real_distribution dist(min, max);

    return dist(e2);
}

int gensokyo::math::random_int(const int min, const int max)
{
    static std::random_device rd;
    static std::mt19937 e2(rd());
    std::uniform_int_distribution dist(min, max);

    return dist(e2);
}

std::array<gensokyo::fVector3, 3> gensokyo::math::angle_vectors(const fAngle& angle)
{
    std::array<fVector3, 3> results;

    auto sine = fVector3(), cosine = fVector3();

    sin_cos(deg_to_rad(angle.x), sine.x, cosine.x);
    sin_cos(deg_to_rad(angle.y), sine.y, cosine.y);
    sin_cos(deg_to_rad(angle.z), sine.z, cosine.z);

    results[0].x = cosine.x * cosine.y;
    results[0].y = cosine.x * sine.y;
    results[0].z = -sine.x;

    results[1].x = -1 * sine.z * sine.x * cosine.y + -1 * cosine.z * -sine.y;
    results[1].y = -1 * sine.z * sine.x * sine.y + -1 * cosine.z * cosine.y;
    results[1].z = -1 * sine.z * cosine.x;

    results[2].x = cosine.z * sine.x * cosine.y + -sine.z * -sine.y;
    results[2].y = cosine.z * sine.x * sine.y + -sine.z * cosine.y;
    results[2].z = cosine.z * cosine.x;

    return results;
}
