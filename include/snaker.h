#ifndef SNAKER_H
# define SNAKER_H

# include <time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h> //for datatypes
# include <string.h> //for sterror
# include <errno.h> //for errno macro
# include <math.h>
# include <stdbool.h>

# include "../libSDL2/include/SDL2/SDL_mixer.h"
# include "../libSDL2/include/SDL2/SDL.h"

# define WIN_W 800
# define WIN_H 600
# define WIN_NAME "Snaker!"

# define TRUE	1
# define FALSE	0

# define DEG_TO_RAD 0.01745329251
# define RAD_TO_DEG 57.2957795131

typedef struct s_buffer {
	uint32_t		*pixels;
	uint32_t		w;
	uint32_t		h;
}					t_buffer;

typedef struct s_rend
{
	SDL_Renderer	*rend;
	SDL_Window		*win;
	SDL_Texture		*win_tex;
	void			*win_pixels;
	t_buffer		*win_buffer;
	int				win_pixel_pitch;
	bool			run;
}					t_rend;

//datatype for handling buffer/array coordinates
typedef struct s_point {
	uint32_t	x;
	uint32_t	y;
}				t_point;

typedef struct s_intpoint {
	int	x;
	int	y;
}				t_intpoint;

typedef struct	s_v3{
	float 	x;
	float	y;
	float	z;
}				t_v3;

typedef struct s_iv3 {
	int			x;
	int			y;
	int			z;
}				t_iv3;

typedef struct s_line {
	t_iv3		start;
	t_iv3		end;
	uint32_t	color;
}				t_line;

typedef	struct	s_keys{
	bool	u;
	bool	d;
	bool	l;
	bool	r;
}				t_keys;

typedef struct	s_snake{
	
	t_keys	k;
}				t_snake;

void		draw_pixel(uint32_t x, uint32_t y, t_buffer *buf, uint32_t color);
void		draw_line(t_buffer *buf, t_point p0, t_point p1, uint32_t color);
void		draw_circle(t_buffer *buf, t_point p, int r, uint32_t color);
void		draw_filled_circle(t_buffer *buf, t_point p, int r, uint32_t color);
void		draw_square(t_point a, t_point b, t_buffer *buf, int color);


#endif
