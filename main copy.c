#include "mlx.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 1920
#define HEIGHT 1080
#define MAX_ITER 100 // 最大反復回数

//#define M_PI 3.141592653589793

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
}					t_data;

int	event_handler(int key, t_data *data)
{
	printf("called\n");
	if (key == XK_Escape)
		mlx_destroy_window(data->mlx, data->win);
	return (0);
}

// マウスが移動したときに呼ばれる関数
int	mouse_move_hook(int x, int y, t_data *data)
{
	printf("Mouse moved to : (%d, %d)\n", x, y);
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

int	interpolate_color(double t)
{
	int	red;
	int	green;
	int	blue;

	red = (int)(sin(t * 2 * M_PI) * 127 + 128);
	green = (int)(sin(t * 2 * M_PI + 2 * M_PI / 3) * 127 + 128);
	blue = (int)(sin(t * 2 * M_PI + 4 * M_PI / 3) * 127 + 128);
	return ((red << 16) | (green << 8) | blue);
}

int	render_frame(t_data *data)
{
	double	t;
	int		color;

	t = (double)(data->frame % 100) / 100.0;
	color = interpolate_color(t);
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			mlx_pixel_put(data->mlx, data->win, x, y, color);
		}
	}
	data->frame++;
	return (0);
}

// void	draw_circle(t_data *img, int cx, int cy, int r, unsigned int color)
// {
// 	for (int y = cy - r; y <= cy + r; y++)
// 	{
// 		for (int x = cx - r; x <= cx + r; x++)
// 		{
// 			// 円の方程式を満たしているかチェック
// 			if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r)
// 			{
// 				my_mlx_pixel_put(img, x, y, color);
// 			}
// 		}
// 	}
// }

// void	draw_line(t_data *img, int x0, int y0, int x1, int y1,
// 		unsigned int color)
// {
// 	int	dx;
// 	int	sx;
// 	int	dy;
// 	int	sy;
// 	int	err;
// 	int	e2;

// 	dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
// 	dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
// 	err = dx + dy;
// 	while (1)
// 	{
// 		my_mlx_pixel_put(img, x0, y0, color);
// 		if (x0 == x1 && y0 == y1)
// 			break ;
// 		e2 = 2 * err;
// 		if (e2 >= dy)
// 		{
// 			err += dy;
// 			x0 += sx;
// 		}
// 		if (e2 <= dx)
// 		{
// 			err += dx;
// 			y0 += sy;
// 		}
// 	}
// }
// void	draw_triangle(t_data *img, int x1, int y1, int x2, int y2, int x3,
// 		int y3, unsigned int color)
// {
// 	draw_line(img, x1, y1, x2, y2, color); // 辺1
// 	draw_line(img, x2, y2, x3, y3, color); // 辺2
// 	draw_line(img, x3, y3, x1, y1, color); // 辺3
// }

int	add_shade(double distance, unsigned color)
{
	int t = (color >> 24) & 0xFF; // Transparency
	int r = (color >> 16) & 0xFF; // Red
	int g = (color >> 8) & 0xFF;  // Green
	int b = color & 0xFF;         // Blue
	// darken the color
	r = r * (1 - distance);
	g = g * (1 - distance);
	b = b * (1 - distance);
	// return new color
	return (t << 24 | r << 16 | g << 8 | b);
}

int	get_opposite(unsigned color)
{
	int t = (color >> 24) & 0xFF; // Transparency
	int r = (color >> 16) & 0xFF; // Red
	int g = (color >> 8) & 0xFF;  // Green
	int b = color & 0xFF;         // Blue
	//補色計算
	r = 255 - r;
	g = 255 - g;
	b = 255 - b;
	// return new color
	return (t << 24 | r << 16 | g << 8 | b);
}

// void	draw_rectangle(t_data *img, int x1, int x2, int y1, int y2)
// {
// 	for (int y = y1; y < y2; y++)
// 	{
// 		for (int x = x1; x < x2; x++)
// 		{
// 			my_mlx_pixel_put(img, x, y, 0x00FF0000);
// 		}
// 	}
// }

int	close_window(t_data *data)
{
	mlx_destroy_window(data->mlx, data->win);
	exit(0);
}

void	draw_julia(t_data *data, double c_re, double c_im)
{
	double	min_re;
	double	min_im;
	double	z_re;
	double	z_im;
	int		iter;
	double	z_re2;
	double	z_im2;
	int		color;
	double	max_re;
	double	max_im;

	min_re = -2.0;
	min_im = -2.0;
	max_re = 2.0;
	max_im = 2.0;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			z_re = min_re + (x / (double)WIDTH) * (max_re - min_re);
			z_im = min_im + (y / (double)HEIGHT) * (max_im - min_im);
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

int	main(void)
{
	t_data			data;
	unsigned int	color2;
	unsigned int	color;

	// t_data			img;
	data.c_re = -0.7;
	data.c_im = 0.37015; // 固定パラメータ（変更可能）
	// t_data			win;
	// t_data			frame;
	data.mlx = mlx_init();
	// 画面出す
	data.win = mlx_new_window(data.mlx, 1920, 1080, "Hello World");
	// 画像を作成
	data.img = mlx_new_image(data.mlx, 1920, 1080);
	// 画像のメモリアドレスを取得
	data.addr = mlx_get_data_addr(data.img, &data.bits_per_pixel,
			&data.line_length, &data.endian);
	// Draw a rectangle
	// draw_rectangle(&data, 100, 300, 100, 400);
	// //色の明度を変える、補色に変える、円をかく
	// color = 0x00FF0000;
	// color = add_shade(0.5, color);
	// draw_circle(mlx.img, 400, 300, 100, color); // 赤色
	// color2 = 0x00FF0000;
	// color2 = get_opposite(color2);
	// draw_circle(mlx.img, 100, 200, 50, color2); // 赤色
	// //三角形
	// draw_triangle(mlx.img, 600, 500, 300, 400, 200, 300, 0x00FF0000);
	// //画像をウィンドウに表示
	draw_julia(&data, data.c_re, data.c_im);
	mlx_put_image_to_window(data.mlx, data.win, data.img, 0, 0);
	mlx_hook(data.win, KeyPress, KeyPressMask, event_handler, data.mlx);
	mlx_hook(data.win, DestroyNotify, StructureNotifyMask, close_window,
		data.mlx);
	//マウス座標表示
	// mlx_hook(mlx.win, MotionNotify, PointerMotionMask, mouse_move_hook,
	//	mlx.mlx);
	//レインボー
	// mlx_loop_hook(mlx.mlx, render_frame, &mlx);
	mlx_loop(data.mlx);
	return (0);
}
