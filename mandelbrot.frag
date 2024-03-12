#version 310 es

precision highp float;

uniform layout(location = 0) int u_iterations;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

#if 0

const uint SHIFT = 24u;
//const int HALF = SHIFT / 2;
//const int MASK = 0x03FFFFFF;
//const int HMASK = 0x0001FFF;

const float MULT = float(1 << SHIFT);

const uint S = uint(16);
const uint M = uint(0xFFFF);

int MakeFixed(float a)
{
    return int(a * MULT);
}

vec2 MakeFixedUvec2(float a)
{
    return vec2(int(a), fract(a));
}

uint Mul(uint a, uint b)
{
    uint h = (a >> S) * (b >> S);

    uint l = (a & M) * (b & M);

    uint hl = (a >> S) * (b & M);

    uint lh = (a & M) * (b >> S);

    h += (hl >> S);
    h += (lh >> S);

    l += (hl << S);
    l += (lh << S);

    //return (h << (32u - SHIFT)) + (l >> SHIFT);
    return (h << (32u - SHIFT)) | (l >> SHIFT);
}

int Mul(int a, int b)
{
#if 0
    int low = (a & HMASK) * (b & HMASK);
    return (((a >> HALF) * (b >> HALF)) & 0xFFFFE000) + low;

    int a_ = a >> SHIFT;
    int b_ = b >> SHIFT;
    int ab = (a_ * b_) << SHIFT;

    int result = (a & MASK) * (b & MASK);
    return (result + ab);
#endif

    return int(Mul(uint(abs(a)),
                   uint(abs(b)))) * sign(a) * sign(b);
}

uvec2 Mul(uvec2 a, uvec2 b)
{
#if 0
    uint a3b3, a3b2, a3b1, a3b0;
    uint a2b3, a2b2, a2b1, a2b0;
    uint a1b3, a1b2, a1b1, a1b0;
    uint a0b3, a0b2, a0b1, a0b0;

    a3b3 = (a.x >> S) * (b.x >> S);

    return uvec2(0);
#else
    uint ah = a.x >> 16;
    uint al = a.x & 0xFFFFu;
    uint bh = b.x >> 16;
    uint bl = b.x & 0xFFFFu;

    uint hh = ah * bh;
    uint ll = al * bl;

    uint hl = ah * bl;
    uint lh = al * bh;

    return uvec2((hh << 16) + (hl >> 16) + (lh >> 16),
                 ll + (hl & 0xFFFFu) + (lh & 0xFFFFu));
#endif
}

ivec2 Mul(ivec2 a, ivec2 b)
{
    return ivec2(Mul(uvec2(abs(a)),
                     uvec2(abs(b)))) * sign(a) * sign(b);
}

void main()
{
#if 0
    int x0 = MakeFixed(texCoord.x);
    int y0 = MakeFixed(texCoord.y);

    int x = 0;
    int y = 0;
    int iteration = 0;
    int xx = 0;
    int yy = 0;

    int fixed4 = MakeFixed(4.0);
    int fixed2 = MakeFixed(2.0);

    while (((xx + yy) <= fixed4) && (iteration < u_iterations)) {
        xx = Mul(x, x);
        yy = Mul(y, y);
        int xtemp = xx - yy + x0;
        y = Mul(Mul(fixed2, x), y) + y0;
        x = xtemp;
        iteration++;
    }
#else
    vec2 x0 = MakeFixedUvec2(texCoord.x);
    vec2 y0 = MakeFixedUvec2(texCoord.y);

    vec2 x = vec2(0);
    vec2 y = vec2(0);
    int iteration = 0;
    vec2 xx = vec2(0);
    vec2 yy = vec2(0);

    vec2 fixed4 = MakeFixedUvec2(4.0);
    vec2 fixed2 = MakeFixedUvec2(2.0);

    while (((xx + yy) <= fixed4) && (iteration < u_iterations)) {
        xx = Mul(x, x);
        yy = Mul(y, y);
        vec2 xtemp = xx - yy + x0;
        y = Mul(Mul(fixed2, x), y) + y0;
        x = xtemp;
        iteration++;
    }
#endif

    fragColor = texture(texSampler, vec2(float(iteration) / float(u_iterations), 0.0));
}

#else

void main()
{
    float x0 = texCoord.x;
    float y0 = texCoord.y;

    float x = 0.0;
    float y = 0.0;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    while (((xx + yy) <= 4.0) && (iteration < u_iterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iteration++;
    }

    fragColor = texture(texSampler, vec2(float(iteration) / float(u_iterations), 0.0));
    //fragColor = texture(texSampler, vec2(float(iteration) / 100.0, 0.0));
}

#endif
