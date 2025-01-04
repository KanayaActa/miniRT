#ifndef MINIRT_H
# define MINIRT_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <math.h>
# include <string.h>

// --- miniLibX用ヘッダ (インストール先によって異なる場合あり) ---
# include <mlx.h>
# include "libft.h"

// マクロ定義
# define WIDTH  800
# define HEIGHT 600
# define EPSILON 1e-6

// 構造体定義
typedef struct s_vec
{
	double x;
	double y;
	double z;
}	t_vec;

typedef struct s_color
{
	double r;
	double g;
	double b;
}	t_color;

typedef struct s_cam
{
	t_vec   pos;      // カメラ位置
	t_vec   dir;      // 向きベクトル (正規化)
	double  fov;      // 視野角 (degree)
}	t_cam;

typedef struct s_light
{
	t_vec    pos;          // 光源位置
	double   brightness;   // [0.0 ~ 1.0]
	t_color  color;        // 今回は未使用にしてもOK (mandatoryではoptional)
}	t_light;

typedef struct s_ambient
{
	double   ratio;  // [0.0 ~ 1.0]
	t_color  color;
}	t_ambient;

typedef enum e_objtype
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER
}	t_objtype;

typedef struct s_sphere
{
	t_vec    center;
	double   radius;
	t_color  color;
}	t_sphere;

typedef struct s_plane
{
	t_vec    point;
	t_vec    normal;   // 正規化
	t_color  color;
}	t_plane;

typedef struct s_cylinder
{
	t_vec    center;
	t_vec    dir;      // 軸の向き (正規化)
	double   radius;   // 直径の半分
	double   height;
	t_color  color;
}	t_cylinder;

// オブジェクト全体を一つの構造体で管理
typedef struct s_object
{
	t_objtype type;
	void     *data; // sphere, plane, cylinderのポインタ
}	t_object;

typedef struct s_scene
{
	t_ambient   amb;
	t_cam       cam;
	t_light     light;

	t_object   *objs;
	int         obj_count;
}	t_scene;

// MLXデータ
typedef struct s_mlx
{
	void    *mlx_ptr;
	void    *win_ptr;
	void    *img_ptr;
	char    *addr;
	int     bpp;
	int     line_len;
	int     endian;
}	t_mlx;

// 全体管理構造体
typedef struct s_data
{
	t_scene scene;
	t_mlx   mlx;
}	t_data;

// --- 関数プロトタイプ ---

// parse.c
int     parse_rt_file(const char *filename, t_scene *scene);

// render.c
void    render_scene(t_data *d);
void    put_pixel(t_mlx *m, int x, int y, t_color c);

// intersect.c
int     intersect_sphere(t_vec ro, t_vec rd, t_sphere *sp, double *t);
int     intersect_plane(t_vec ro, t_vec rd, t_plane *pl, double *t);
int     intersect_cylinder(t_vec ro, t_vec rd, t_cylinder *cy, double *t);

// utils.c (ベクトル演算など)
t_vec   vec_sub(t_vec a, t_vec b);
t_vec   vec_add(t_vec a, t_vec b);
t_vec   vec_scale(t_vec a, double s);
double  vec_dot(t_vec a, t_vec b);
t_vec   vec_cross(t_vec a, t_vec b);
double  vec_len(t_vec a);
t_vec   vec_norm(t_vec a);
t_vec   make_vec(double x, double y, double z);
t_color make_color(double r, double g, double b);
t_color color_clamp(t_color c);

// イベント関連
int     close_window(t_data *d);
int     key_press(int keycode, t_data *d);

// エラー処理
int     error_exit(const char *msg);

#endif
