float IsPointInAABB(vec3 p, vec3 center, vec3 halfSize)
{
    vec3 d = abs(p - center) - halfSize;
    return step(0.0, -max(d.x, max(d.y, d.z))); // returns 1.0 if inside, 0.0 if outside
}

float IsPointInSphere(vec3 p, vec3 center, float radius)
{
    float distSq = dot(p - center, p - center);
    return step(distSq, radius * radius); // 1.0 if inside, 0.0 if outside
}
