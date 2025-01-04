#include "miniRT.h"

int close_window(t_data *d)
{
	// 必要に応じてリソース解放
	mlx_destroy_window(d->mlx.mlx_ptr, d->mlx.win_ptr);
	exit(0);
	return (0);
}

int key_press(int keycode, t_data *d)
{
	if (keycode == 65307) // ESC (macOSの場合)
		close_window(d);
	// Windows, LinuxだとESCキーコードが異なる: 65307 など
	return (0);
}

int error_exit(const char *msg)
{
	printf("Error\n%s\n", msg);
	return (1);
}
