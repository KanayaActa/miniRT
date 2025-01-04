#include "miniRT.h"

// 文字列を","でsplitしてベクトルに変換
static int parse_vector(char *str, t_vec *v)
{
	char **xyz = NULL;

	xyz = ft_split(str, ','); // libftの想定関数/あるいは自前splitでもOK
	if (!xyz || !xyz[0] || !xyz[1] || !xyz[2])
		return (0);
	v->x = atof(xyz[0]);
	v->y = atof(xyz[1]);
	v->z = atof(xyz[2]);
	// free xyz...
	return (1);
}

// R,G,B のパース
static int parse_color(char *str, t_color *c)
{
	char **rgb = ft_split(str, ',');
	if (!rgb || !rgb[0] || !rgb[1] || !rgb[2])
		return (0);
	c->r = atof(rgb[0]);
	c->g = atof(rgb[1]);
	c->b = atof(rgb[2]);
	// free rgb...
	return (1);
}

// オブジェクト追加用 (簡易)
static void add_object(t_scene *s, t_objtype type, void *data)
{
	int idx = s->obj_count;
	// メモリ再確保など省略 or 動的配列前提
	s->objs[idx].type = type;
	s->objs[idx].data = data;
	s->obj_count++;
}

// Ambient "A 0.2 255,255,255"
static int parse_ambient(char **tokens, t_scene *scene)
{
	scene->amb.ratio = atof(tokens[1]);
	if (!parse_color(tokens[2], &scene->amb.color))
		return (0);
	return (1);
}

// Camera "C -50.0,0,20 0,0,1 70"
static int parse_camera(char **tokens, t_scene *scene)
{
	parse_vector(tokens[1], &scene->cam.pos);
	parse_vector(tokens[2], &scene->cam.dir);
	scene->cam.fov = atof(tokens[3]);
	// dirを正規化
	scene->cam.dir = vec_norm(scene->cam.dir);
	return (1);
}

// Light "L -40.0,50.0,0.0 0.6 255,255,255"
static int parse_light(char **tokens, t_scene *scene)
{
	parse_vector(tokens[1], &scene->light.pos);
	scene->light.brightness = atof(tokens[2]);
	if (tokens[3])
		parse_color(tokens[3], &scene->light.color); // mandatoryで未使用OK
	return (1);
}

// Sphere "sp 0.0,0.0,20.6 12.6 10,0,255"
static int parse_sphere(char **tokens, t_scene *scene)
{
	t_sphere *sp;

	sp = malloc(sizeof(t_sphere));
	if (!sp)
		return (0);
	parse_vector(tokens[1], &sp->center);
	sp->radius = atof(tokens[2]) / 2.0;
	parse_color(tokens[3], &sp->color);
	add_object(scene, OBJ_SPHERE, sp);
	return (1);
}

// Plane "pl 0.0,0.0,-10.0 0.0,1.0,0.0 0,0,225"
static int parse_plane(char **tokens, t_scene *scene)
{
	t_plane *pl;

	pl = malloc(sizeof(t_plane));
	parse_vector(tokens[1], &pl->point);
	parse_vector(tokens[2], &pl->normal);
	pl->normal = vec_norm(pl->normal);
	parse_color(tokens[3], &pl->color);
	add_object(scene, OBJ_PLANE, pl);
	return (1);
}

// Cylinder "cy 50.0,0.0,20.6 0.0,0.0,1.0 14.2 21.42 10,0,255"
static int parse_cylinder(char **tokens, t_scene *scene)
{
	t_cylinder *cy;

	cy = malloc(sizeof(t_cylinder));
	parse_vector(tokens[1], &cy->center);
	parse_vector(tokens[2], &cy->dir);
	cy->dir = vec_norm(cy->dir);
	cy->radius = atof(tokens[3]) / 2.0;
	cy->height = atof(tokens[4]);
	parse_color(tokens[5], &cy->color);
	add_object(scene, OBJ_CYLINDER, cy);
	return (1);
}

int parse_rt_file(const char *filename, t_scene *scene)
{
	int    fd;
	char   *line;

	// とりあえずobjsを固定配列と仮定 (最大100個)
	scene->objs = malloc(sizeof(t_object) * 100);
	scene->obj_count = 0;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (0);

	while ((line = get_next_line(fd))) // あるいは自前で行読み取り
	{
		char **tokens = ft_split(line, ' ');
		if (!tokens[0]) // 空行など
		{
			// free...
			continue;
		}
		if (strcmp(tokens[0], "A") == 0)
			parse_ambient(tokens, scene);
		else if (strcmp(tokens[0], "C") == 0)
			parse_camera(tokens, scene);
		else if (strcmp(tokens[0], "L") == 0)
			parse_light(tokens, scene);
		else if (strcmp(tokens[0], "sp") == 0)
			parse_sphere(tokens, scene);
		else if (strcmp(tokens[0], "pl") == 0)
			parse_plane(tokens, scene);
		else if (strcmp(tokens[0], "cy") == 0)
			parse_cylinder(tokens, scene);
		else
		{
			// 未知の識別子 → エラー
			free(line);
			close(fd);
			return (0);
		}
		free(line);
	}
	close(fd);
	return (1);
}
