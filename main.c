#include "mlx.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 1920
#define HEIGHT 1080
#define MAX_ITER 100    // 最大反復回数
#define ZOOM_FACTOR 1.2 // ズーム倍率

typedef struct s_data
{
	void			*mlx;
	void			*img;
	void			*win;
	unsigned char	*addr;
	int				bits_per_pixel;
	int				line_length;
	int				endian;
	int				frame;
	double			c_re;
	double			c_im;
	double			min_re;
	double			max_re;
	double			min_im;
	double			max_im;
}					t_data;

int	event_handler(int key, t_data *data)
{
	printf("called\n");
	if (key == XK_Escape)
		mlx_destroy_window(data->mlx, data->win);
	return (0);
}

void	my_mlx_pixel_put(t_data *data, int x, int y, unsigned int color)
{
	char	*dst;

	// 画像のメモリアドレスの計算式
	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	//計算した dst の位置にピクセルの色 (color) を書き込む
	*(unsigned int *)dst = color;
}

int	close_window(t_data *data)
{
	mlx_destroy_window(data->mlx, data->win);
	exit(0);
}

void	draw_julia(t_data *data)
{
	double	z_re;
	double	z_im;
	int		iter;
	double	z_re2;
	double	z_im2;
	int		color;

	// double	min_re;
	// double	min_im;
	// double	max_re;
	// double	max_im;
	data->min_re = -2.0;
	data->min_im = -2.0;
	data->max_re = 2.0;
	data->max_im = 2.0;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			z_re = data->min_re + (x / (double)WIDTH) * (data->max_re
					- data->min_re);
			z_im = data->min_im + (y / (double)HEIGHT) * (data->max_im
					- data->min_im);
			for (iter = 0; iter < MAX_ITER; iter++)
			{
				z_re2 = z_re * z_re - z_im * z_im + data->c_re;
				z_im2 = 2 * z_re * z_im + data->c_im;
				z_re = z_re2;
				z_im = z_im2;
				if (z_re * z_re + z_im * z_im > 4)
					break ;
			}
			color = (iter == MAX_ITER) ? 0x000000 : (iter * 255
					/ MAX_ITER) << 8;
			my_mlx_pixel_put(data, x, y, color);
		}
	}
}
int	mouse_zoom(int button, int x, int y, t_data *data)
{
	double	zoom;
	double	mouse_re;
	double	mouse_im;

	if (button == 4 || button == 5) // 4: 上スクロール (ズームイン), 5: 下スクロール (ズームアウト)
	{
		zoom = (button == 4) ? (1.0 / ZOOM_FACTOR) : ZOOM_FACTOR;
		// マウスのスクリーン座標をフラクタル座標に変換
		mouse_re = data->min_re + (x / (double)WIDTH) * (data->max_re
				- data->min_re);
		mouse_im = data->min_im + (y / (double)HEIGHT) * (data->max_im
				- data->min_im);
		// 範囲を縮小・拡大
		data->min_re = mouse_re + (data->min_re - mouse_re) * zoom;
		data->max_re = mouse_re + (data->max_re - mouse_re) * zoom;
		data->min_im = mouse_im + (data->min_im - mouse_im) * zoom;
		data->max_im = mouse_im + (data->max_im - mouse_im) * zoom;
		draw_julia(data);
	}
	return (0);
}
int	main(void)
{
	t_data			data;
	unsigned int	color2;
	unsigned int	color;

	// t_data			img;
	data.c_re = -0.7;
	data.c_im = 0.37015; // 固定パラメータ（変更可能）
	data.mlx = mlx_init();
	// 画面出す
	data.win = mlx_new_window(data.mlx, 1920, 1080, "Hello World");
	// 画像を作成
	data.img = mlx_new_image(data.mlx, 1920, 1080);
	// 画像のメモリアドレスを取得
	data.addr = mlx_get_data_addr(data.img, &data.bits_per_pixel,
			&data.line_length, &data.endian);
	// //画像をウィンドウに表示
	draw_julia(&data);
	mlx_put_image_to_window(data.mlx, data.win, data.img, 0, 0);
	mlx_hook(data.win, KeyPress, KeyPressMask, event_handler, &data);
	mlx_hook(data.win, DestroyNotify, StructureNotifyMask, close_window, &data);
	mlx_mouse_hook(data.win, mouse_zoom, &data);
	mlx_loop(data.mlx);
	return (0);
}
