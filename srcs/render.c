#include "miniRT.h"

static t_color compute_lighting(t_scene *scene, t_vec ro, t_vec rd, double tmin, t_object *hit_obj)
{
	t_color out = make_color(0, 0, 0);
	t_vec   phit = vec_add(ro, vec_scale(rd, tmin)); // 交差点
	t_vec   normal;
	t_color obj_color;
	int     in_shadow = 0;

	// オブジェクトタイプに応じて法線や色を取得
	if (hit_obj->type == OBJ_SPHERE)
	{
		t_sphere *sp = (t_sphere*)hit_obj->data;
		normal = vec_norm(vec_sub(phit, sp->center));
		obj_color = sp->color;
	}
	else if (hit_obj->type == OBJ_PLANE)
	{
		t_plane *pl = (t_plane*)hit_obj->data;
		normal = pl->normal;
		obj_color = pl->color;
	}
	else
	{
		t_cylinder *cy = (t_cylinder*)hit_obj->data;
		// 円柱の側面の法線を厳密に計算するには工夫が必要。
		// 簡易的に中心->phitベクトルから軸成分を除去するなど。
		// ここでは詳細実装省略(要実装)
		obj_color = cy->color;
		normal = make_vec(0, 1, 0); // 仮
	}

	// アンビエント
	t_color amb = make_color(scene->amb.color.r * scene->amb.ratio,
		scene->amb.color.g * scene->amb.ratio,
		scene->amb.color.b * scene->amb.ratio);
	out.r += amb.r * (obj_color.r / 255.0);
	out.g += amb.g * (obj_color.g / 255.0);
	out.b += amb.b * (obj_color.b / 255.0);

	// シャドウ判定
	{
		t_vec light_dir = vec_sub(scene->light.pos, phit);
		double dist_light = vec_len(light_dir);
		light_dir = vec_norm(light_dir);

		// シャドウ用の微小オフセット
		t_vec phit_off = vec_add(phit, vec_scale(normal, EPSILON * 10));
		double t_shadow_min = 1e30;
		t_object *shadow_obj = NULL;

		// シーン中のオブジェクトと交差をチェック
		for (int i = 0; i < scene->obj_count; i++)
		{
			double ttemp;
			int intersected = 0;

			if (scene->objs[i].type == OBJ_SPHERE)
				intersected = intersect_sphere(phit_off, light_dir, (t_sphere*)scene->objs[i].data, &ttemp);
			else if (scene->objs[i].type == OBJ_PLANE)
				intersected = intersect_plane(phit_off, light_dir, (t_plane*)scene->objs[i].data, &ttemp);
			else if (scene->objs[i].type == OBJ_CYLINDER)
				intersected = intersect_cylinder(phit_off, light_dir, (t_cylinder*)scene->objs[i].data, &ttemp);

			if (intersected && ttemp > EPSILON && ttemp < dist_light)
			{
				shadow_obj = &scene->objs[i];
				t_shadow_min = ttemp;
				break;
			}
		}
		if (shadow_obj)
			in_shadow = 1;
	}

	// ディフューズ
	if (!in_shadow)
	{
		t_vec light_dir = vec_norm(vec_sub(scene->light.pos, phit));
		double dot = vec_dot(normal, light_dir);
		if (dot < 0) dot = 0;
		double diff = dot * scene->light.brightness;

		out.r += (obj_color.r / 255.0) * diff * scene->light.color.r;
		out.g += (obj_color.g / 255.0) * diff * scene->light.color.g;
		out.b += (obj_color.b / 255.0) * diff * scene->light.color.b;

		// 光源の色を使用しないなら
		// out.r += (obj_color.r / 255.0) * diff * 255;
		// out.g += (obj_color.g / 255.0) * diff * 255;
		// out.b += (obj_color.b / 255.0) * diff * 255;
	}

	return color_clamp(out);
}

void render_scene(t_data *d)
{
	t_scene *scene = &d->scene;

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// NDC
			double u = (double)x / (double)WIDTH;
			double v = (double)(HEIGHT - y - 1) / (double)HEIGHT;

			// レイ生成 (簡易ピンホールモデル)
			double fov_scale = tan((scene->cam.fov * 0.5 * M_PI / 180.0));
			double aspect = (double)WIDTH / (double)HEIGHT;
			t_vec rd;
			rd.x = (2 * u - 1) * aspect * fov_scale;
			rd.y = (2 * v - 1) * fov_scale;
			rd.z = 1; // カメラの前方向をz+と仮定
			// カメラのdirを回転に使う場合は行列変換が必要 (省略)

			rd = vec_norm(rd);
			t_vec ro = scene->cam.pos;

			// シーン中の一番近いオブジェクトを探す
			double t_near = 1e30;
			t_object *hit_obj = NULL;

			for (int i = 0; i < scene->obj_count; i++)
			{
				double ttemp;
				int intersected = 0;
				if (scene->objs[i].type == OBJ_SPHERE)
					intersected = intersect_sphere(ro, rd, (t_sphere*)scene->objs[i].data, &ttemp);
				else if (scene->objs[i].type == OBJ_PLANE)
					intersected = intersect_plane(ro, rd, (t_plane*)scene->objs[i].data, &ttemp);
				else if (scene->objs[i].type == OBJ_CYLINDER)
					intersected = intersect_cylinder(ro, rd, (t_cylinder*)scene->objs[i].data, &ttemp);

				if (intersected && ttemp < t_near && ttemp > EPSILON)
				{
					t_near = ttemp;
					hit_obj = &scene->objs[i];
				}
			}

			t_color pixel_color = make_color(0, 0, 0);
			if (hit_obj)
			{
				pixel_color = compute_lighting(scene, ro, rd, t_near, hit_obj);
			}
			else
			{
				// 背景色
				pixel_color = make_color(0, 0, 0);
			}

			put_pixel(&d->mlx, x, y, pixel_color);
		}
	}
}

// ピクセル描画
void put_pixel(t_mlx *m, int x, int y, t_color c)
{
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return;
	int   offset = (y * m->line_len) + (x * (m->bpp / 8));
	int   r = (int)c.r;
	int   g = (int)c.g;
	int   b = (int)c.b;
	m->addr[offset + 0] = b;  // BGRA順 or RGBA順か環境次第
	m->addr[offset + 1] = g;
	m->addr[offset + 2] = r;
}

