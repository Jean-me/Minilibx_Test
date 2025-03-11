#include "mlx.h"
#include <X11/X.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_data
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}			t_data;

int	event_handler(int key, void *mlx)
{
	printf("called\n");
	return (0);
}

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	// 画像のメモリアドレスの計算式
	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	//計算した dst の位置にピクセルの色 (color) を書き込む
	*(unsigned int *)dst = color;
}

void	draw_circle(t_data *img, int cx, int cy, int r, int color)
{
	for (int y = cy - r; y <= cy + r; y++)
	{
		for (int x = cx - r; x <= cx + r; x++)
		{
			// 円の方程式を満たしているかチェック
			if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r)
			{
				my_mlx_pixel_put(img, x, y, color);
			}
		}
	}
}

void	draw_line(t_data *img, int x0, int y0, int x1, int y1, int color)
{
	int	dx;
	int	sx;
	int	dy;
	int	sy;
	int	err;
	int	e2;

	dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	err = dx + dy;
	while (1)
	{
		my_mlx_pixel_put(img, x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break ;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}
void	draw_triangle(t_data *img, int x1, int y1, int x2, int y2, int x3,
		int y3, int color)
{
	draw_line(img, x1, y1, x2, y2, color); // 辺1
	draw_line(img, x2, y2, x3, y3, color); // 辺2
	draw_line(img, x3, y3, x1, y1, color); // 辺3
}

int	main(void)
{
	void	*mlx;
	void	*mlx_win;
	t_data	img;

	mlx = mlx_init();
	// 画面出す
	mlx_win = mlx_new_window(mlx, 1920, 1080, "Hello World");
	// 画像を作成
	img.img = mlx_new_image(mlx, 1920, 1080);
	// 画像のメモリアドレスを取得
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
			&img.endian);
	//どこにピクセルを書き込むか計算
	// my_mlx_pixel_put(&img, 500, 500, 0x00FF0000);
	for (int y = 100; y < 200; y++)
	{
		for (int x = 100; x < 520; x++)
		{
			my_mlx_pixel_put(&img, x, y, 0x00FF0000);
		}
	}
	//円をかく
	draw_circle(&img, 400, 300, 100, 0x00FF0000); // 赤色
	//三角形
	draw_triangle(&img, 600, 600, 300, 300, 200, 200, 0x00FF0000);
	//画像をウィンドウに表示
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	mlx_hook(mlx_win, KeyPress, KeyPressMask, event_handler, mlx);
	mlx_loop(mlx);
	return (0);
}
