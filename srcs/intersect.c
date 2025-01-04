#include "miniRT.h"

// 球との交差判定
int intersect_sphere(t_vec ro, t_vec rd, t_sphere *sp, double *t)
{
	// (ro + rd*t - center)^2 = r^2
	t_vec oc = vec_sub(ro, sp->center);
	double a = vec_dot(rd, rd);
	double b = 2.0 * vec_dot(oc, rd);
	double c = vec_dot(oc, oc) - sp->radius * sp->radius;

	double discriminant = b*b - 4*a*c;
	if (discriminant < 0)
		return (0);
	double sqrt_d = sqrt(discriminant);

	double t0 = (-b - sqrt_d) / (2*a);
	double t1 = (-b + sqrt_d) / (2*a);
	if (t0 > t1)
	{
		double tmp = t0; t0 = t1; t1 = tmp;
	}
	if (t1 < 0) // 2つとも負
		return (0);

	double hit_t = (t0 < 0) ? t1 : t0;
	*t = hit_t;
	return (1);
}

// 平面との交差判定
int intersect_plane(t_vec ro, t_vec rd, t_plane *pl, double *t)
{
	double denom = vec_dot(pl->normal, rd);
	if (fabs(denom) < EPSILON)
		return (0);
	double d = vec_dot(pl->normal, vec_sub(pl->point, ro)) / denom;
	if (d < 0)
		return (0);
	*t = d;
	return (1);
}

// 円柱との交差判定 (簡易版/側面のみ)
int intersect_cylinder(t_vec ro, t_vec rd, t_cylinder *cy, double *t)
{
	// 実装は省略的に
	// 厳密には軸方向成分を除いた rd, ro を計算して二次方程式を解く必要がある
	// また円柱の上下キャップを判定するなら平面交差も追加で要る
	// ここではダミー実装として常に0を返す
	(void)ro;
	(void)rd;
	(void)cy;
	(void)t;
	return (0);
}
