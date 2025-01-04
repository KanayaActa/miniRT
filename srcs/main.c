#include "miniRT.h"

static void init_mlx(t_data *d)
{
	d->mlx.mlx_ptr = mlx_init();
	if (!d->mlx.mlx_ptr)
		error_exit("mlx_init failed");

	d->mlx.win_ptr = mlx_new_window(d->mlx.mlx_ptr, WIDTH, HEIGHT, "miniRT");
	if (!d->mlx.win_ptr)
		error_exit("mlx_new_window failed");

	d->mlx.img_ptr = mlx_new_image(d->mlx.mlx_ptr, WIDTH, HEIGHT);
	if (!d->mlx.img_ptr)
		error_exit("mlx_new_image failed");

	d->mlx.addr = mlx_get_data_addr(d->mlx.img_ptr,
		&d->mlx.bpp, &d->mlx.line_len, &d->mlx.endian);
}

// エントリーポイント
int	main(int argc, char **argv)
{
	t_data d;

	if (argc != 2)
		return (error_exit("Usage: ./miniRT <scene.rt>"));

	// シーン構造体を初期化
	memset(&d, 0, sizeof(t_data));

	// .rtファイルの読み込み & パース
	if (!parse_rt_file(argv[1], &d.scene))
		return (error_exit("Failed to parse .rt file"));

	// MLX初期化
	init_mlx(&d);

	// シーンの描画
	render_scene(&d);

	// ウィンドウにイメージを表示
	mlx_put_image_to_window(d.mlx.mlx_ptr, d.mlx.win_ptr, d.mlx.img_ptr, 0, 0);

	// フック関数の設定
	mlx_hook(d.mlx.win_ptr, 17, 0, close_window, &d);       // ウィンドウの × ボタン
	mlx_key_hook(d.mlx.win_ptr, key_press, &d);             // キー入力
	mlx_loop(d.mlx.mlx_ptr);

	return (0);
}
