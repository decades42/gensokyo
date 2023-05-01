#pragma once

namespace gensokyo::math
{
    float rad_to_deg(float rad);
    float deg_to_rad(float deg);
    float normalize_yaw(float yaw, float range = 360.f);
    void sin_cos(float rad, float& sine, float& cosine);
    void transform(const fVector3& tr, Matrix3x4& matrix, fVector3& out);
    std::array<fVector3, 3> angle_vectors(const fAngle& angle);
    float random_float(float min, float max);
    int random_int(int min, int max);
}
