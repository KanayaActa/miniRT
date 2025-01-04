#include "miniRT.h"

t_vec make_vec(double x, double y, double z)
{
	t_vec v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

t_color make_color(double r, double g, double b)
{
	t_color c;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

t_color color_clamp(t_color c)
{
	if (c.r < 0) c.r = 0; if (c.r > 255) c.r = 255;
	if (c.g < 0) c.g = 0; if (c.g > 255) c.g = 255;
	if (c.b < 0) c.b = 0; if (c.b > 255) c.b = 255;
	return c;
}

t_vec vec_sub(t_vec a, t_vec b)
{
	return make_vec(a.x - b.x, a.y - b.y, a.z - b.z);
}

t_vec vec_add(t_vec a, t_vec b)
{
	return make_vec(a.x + b.x, a.y + b.y, a.z + b.z);
}

t_vec vec_scale(t_vec a, double s)
{
	return make_vec(a.x * s, a.y * s, a.z * s);
}

double vec_dot(t_vec a, t_vec b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

t_vec vec_cross(t_vec a, t_vec b)
{
	return make_vec(a.y*b.z - a.z*b.y,
	                a.z*b.x - a.x*b.z,
	                a.x*b.y - a.y*b.x);
}

double vec_len(t_vec a)
{
	return sqrt(vec_dot(a, a));
}

t_vec vec_norm(t_vec a)
{
	double len = vec_len(a);
	if (len < EPSILON)
		return make_vec(0, 0, 0); // 0ベクトルは正規化できない
	return make_vec(a.x/len, a.y/len, a.z/len);
}
