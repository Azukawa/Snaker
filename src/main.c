#include "snaker.h"

void	ft_getout(const char *s)
{
	size_t	i;
	char	*c;

	if (s != NULL)
	{
		c = "\n";
		i = 0;
		while (s[i] != '\0')
			i++;
		write(2, s, i);
		write(2, c, 1);
	}
	exit(EXIT_FAILURE);
}

static void	init(t_rend *renderer, t_snake *snake)
{
	bzero(snake, sizeof(t_snake));

	bzero(renderer, sizeof(t_rend));
	renderer->win_buffer = (t_buffer *)malloc(sizeof(t_buffer));
	if (!renderer->win_buffer)
		ft_getout("failed to initialize main buffer");
	renderer->win_buffer->w = WIN_W;
	renderer->win_buffer->h = WIN_H;
	renderer->win_buffer->pixels = (uint32_t *)malloc(sizeof(uint32_t) * WIN_H * WIN_W);
	if (!renderer->win_buffer->pixels)
		ft_getout("Failed to allocate pixel buffer");
	renderer->win_buffer->pitch = WIN_W;
	bzero(renderer->win_buffer->pixels, sizeof(uint32_t) * WIN_H * WIN_W);
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		ft_getout(SDL_GetError());
	renderer->win = SDL_CreateWindow(WIN_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_W, WIN_H, 0);
	if (!renderer->win)
		ft_getout(SDL_GetError());
	renderer->rend = SDL_CreateRenderer(renderer->win, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer->rend)
		ft_getout(SDL_GetError());
	renderer->win_tex = SDL_CreateTexture(renderer->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIN_W, WIN_H);
	if (!renderer->win_tex)
		ft_getout(SDL_GetError());
	renderer->run = TRUE;
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048 ) != 0)
		ft_getout(SDL_GetError());
	printf("Init success.\n");
}

static void	cleanup(t_rend *renderer)
{
	SDL_DestroyTexture(renderer->win_tex);
	SDL_DestroyRenderer(renderer->rend);
	SDL_DestroyWindow(renderer->win);
	free(renderer->win_buffer->pixels);
	free(renderer->win_buffer);
	Mix_Quit();
	SDL_Quit();
}

t_iv3	v32iv3(t_v3 v)
{
	t_iv3	ret;

	ret.x = v.x;
	ret.y = v.y;
	ret.z = v.z;
	return (ret);
}

t_point	v2p(t_v3 v)
{
	t_point	ret;
	
	ret.x = v.x;
	ret.y = v.y;
	return (ret);
}

void backend_checker(t_rend *rend)
{
	// check for gpu
	SDL_RendererInfo info;
	SDL_GetRendererInfo(rend->rend, &info);
	printf("Renderer backend: %s\n", info.name);
}

void draw_2_window(t_rend *rend)
{
    if (SDL_UpdateTexture(rend->win_tex, NULL, rend->win_buffer->pixels, WIN_W * 4) < 0)
		ft_getout(SDL_GetError());

    SDL_RenderCopy(rend->rend, rend->win_tex, NULL, NULL);
    SDL_RenderPresent(rend->rend);
}

void	keyevent(SDL_Event *e, t_rend *rend, t_snake *snake)
{
	while (SDL_PollEvent(e))
	{
		if (e->window.event == SDL_WINDOWEVENT_CLOSE || e->key.keysym.sym == SDLK_ESCAPE)
			rend->run = FALSE;
		if (e->key.keysym.sym == SDLK_UP && e->type == SDL_KEYDOWN)
			snake->k.u = 1;
		else if (e->key.keysym.sym == SDLK_UP && e->type == SDL_KEYUP)
			snake->k.u = 0;
		if (e->key.keysym.sym == SDLK_DOWN && e->type == SDL_KEYDOWN)
			snake->k.d = 1;
		else if (e->key.keysym.sym == SDLK_DOWN && e->type == SDL_KEYUP)
			snake->k.d = 0;
		if (e->key.keysym.sym == SDLK_LEFT && e->type == SDL_KEYDOWN)
			snake->k.l = 1;
		else if (e->key.keysym.sym == SDLK_LEFT && e->type == SDL_KEYUP)
			snake->k.l = 0;
		if (e->key.keysym.sym == SDLK_RIGHT && e->type == SDL_KEYDOWN)
			snake->k.r = 1;
		else if (e->key.keysym.sym == SDLK_RIGHT && e->type == SDL_KEYUP)
			snake->k.r = 0;
	}
}

uint32_t	get_delta()
{
	static uint32_t	tics;
	uint32_t			pretics;

	pretics = tics;
	tics = SDL_GetTicks();
	return((tics - pretics));
}

//Calculate magnitude of vector
float v3_mag(t_v3 vec)
{
    float sum = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    return sqrtf(sum);
}

t_v3	v3_add(t_v3 a, t_v3 b)
{
	a.x = a.x + b.x;
	a.y = a.y + b.y;
	a.z = a.z + b.z;
	return(a);
}

t_v3   v_sub(t_v3 a, t_v3 b)
{
	t_v3   ret;

	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;
	return (ret);
}
  
t_v3   v_div(t_v3 vector, float c)
{
	vector.x = vector.x / c;
	vector.y = vector.y / c;
	vector.z = vector.z / c;
	return (vector);
}
  
t_v3   v_mult(t_v3 vector, float c)
{
	vector.x = vector.x * c;
	vector.y = vector.y * c;
	vector.z = vector.z * c;
	return (vector);
}

void	v_rot_x(t_v3 *vec, float rad)
{
	t_v3	prev;

	prev.z = vec->z;
	prev.y = vec->y;
	vec->z = prev.z * cos(rad) - prev.y * sin(rad);
	vec->y = prev.z * sin(rad) + prev.y * cos(rad);
}

void	v_rot_y(t_v3 *vec, float rad)
{
	t_v3	prev;

	prev.x = vec->x;
	prev.z = vec->z;
	vec->x = prev.x * cos(rad) - prev.z * sin(rad);
	vec->z = prev.x * sin(rad) + prev.z * cos(rad);
}

void	v_rot_z(t_v3 *vec, float rad)
{
	t_v3	prev;

	prev.y = vec->y;
	prev.x = vec->x;
	vec->x = prev.x * cos(rad) - prev.y * sin(rad);
	vec->y = prev.x * sin(rad) + prev.y * cos(rad);
}

t_v3	v_rot_xyz(t_v3 vec, t_v3 rot)
{
	t_v3	ret;

	ret = vec;
	v_rot_x(&ret, DEG_TO_RAD * rot.x);
	v_rot_y(&ret, DEG_TO_RAD * rot.y);
	v_rot_z(&ret, DEG_TO_RAD * rot.z);
	return (ret);
}

t_v3	v3_cross(t_v3 va, t_v3 vb)
{
	t_v3	result;

	result.x = va.y * vb.z - va.z * vb.y;
	result.y = va.z * vb.x - va.x * vb.z;
	result.z = va.x * vb.y - va.y * vb.x;
	return (result);
}

float	v3_len(t_v3 v)
{
		return (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
}

float   v3_dot(t_v3 va, t_v3 vb)
{
    return (va.x * vb.x + va.y * vb.y + va.z * vb.z);
}

float   v2_dot(t_v3 va, t_v3 vb)
{
    return (va.x * vb.x + va.y * vb.y);
}

t_v3	rad2vec3(float dir)
{
	t_v3	ret = {0,0,0};
	
	ret.x = cos(dir);
	ret.y = sin(dir);
	return (ret);
}
float	lerp_1d(float start, float end, float t)
{
		return (start + t * (end - start));
}

t_v3	v3_lerp(t_v3 va, t_v3 vb, float t)
{
	t_v3	ret = {0,0,0};
	
	ret.x = lerp_1d(va.x, vb.x, t);
	ret.y = lerp_1d(va.y, vb.y, t);
	ret.z = lerp_1d(va.z, vb.z, t);
	return (ret);
}


/*
*	Multiplying negative values with themselves will always result in positive
*	values, therefore sqrt() call will always success.
*	(Additionally, it does not trip onto possible NaN / Inf values.)
*/
float	v_len(t_v3 v)
{
	return (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
}

t_v3	v_normalize(t_v3 v)
{
	t_v3	ret;
	float		l;

	l = v_len(v);
	ret.x = v.x / l;
	ret.y = v.y / l;
	ret.z = v.z / l;
	return (ret);
}

float my_max(float a, float b)
{
	if(a > b)
		return(a);
	return(b);
}

t_v3 v3_scale(t_v3 v, float scalar)
{
    t_v3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

t_point		intpoint_2_point(t_intpoint p1)
{
	t_point p2;
	p2.x = (uint32_t)p1.x;
	p2.y = (uint32_t)p1.y;
	return(p2);
}

// this is dumb, make to use clamp
t_point	limit_point_to_screen(t_intpoint p)
{
	if (p.x < 0)
		p.x = 0;
	if (p.x > WIN_W)
		p.x = WIN_W;
	if (p.y < 0)
		p.y = 0;
	if (p.y > WIN_H)
		p.y = WIN_H;
	t_point p_ret;
	p_ret.x = (uint32_t)p.x;
	p_ret.y = (uint32_t)p.y;

	return(p_ret);
}

int		ft_clamp(int min, int max, int nb)
{
	if (nb < min)
		return (min);
	if (nb > max)
		return (max);
	return (nb);
}

void	fps_counter()
{
		static int		i;
		static size_t	sec;
		static size_t	presec;

		sec = clock() / CLOCKS_PER_SEC;
		if (sec == presec)
		{
			i++;
		}
		else
		{
			printf("%dFPS\n", i);
			presec = sec;
			i = 0;
		}	
}

t_point	perspective(t_v3 pos)
{
	t_point ret;
	ret.x = ((pos.x / pos.z) + 1) / 2 * WIN_W;
	ret.y = (1 - (((pos.y / pos.z) + 1)) / 2) * WIN_H;
	return (ret);
}

int		color_brightness(int color, int brightness)
{
	if(brightness > 255)
		brightness = 255;
	if (brightness < 0)
		brightness = 0;

	uint8_t r, g, b;
	r = ((color >> 16 & 0xff) * brightness) >> 8;
	g = ((color >> 8 & 0xff) * brightness) >> 8;
	b = ((color  & 0xff) * brightness) >> 8;
	return(r << 16 | g << 8 | b);
}

// tsekkaa että piirretyt pallot sopii snake leniin, ettei tallenneta turhaan palloja joita ei käytetä
//	värin tummennus-etäisyys funktio
// Onko 'snake_body[0] = pos;' oikeessa paikassa? Pitäiskö olla alussa
// Onko nopeempia keinoja swapata/shiftata koko käärme?
void	draw_snake(t_rend *rend, t_v3 pos)
{
	int				snake_len		= 650;
	static	t_v3	snake_body[650];
	int				index			= snake_len - 2;
	int				body_size		= 50;
	float			size_increment	= (float)body_size / snake_len;
	int				col				= 0xfff0f0;
//	int				col				= 0xff0000;

	while(index >= 0)
	{
		snake_body[index + 1].x = snake_body[index].x;
		snake_body[index + 1].y = snake_body[index].y;
		snake_body[index + 1].z = snake_body[index].z;
		if(index % 15 == 0)
		{
			t_point pos_on_screen = perspective(snake_body[index]);	
			draw_circle(rend->win_buffer, pos_on_screen, (body_size - (size_increment * index))/ snake_body[index].z, color_brightness(col, 255 - snake_body[index].z * 28));
		}
//		col += 145;
		index--;
	}
	snake_body[0] = pos;
}

t_v3	snake_movement(float speed, int mode)
{

	float	wobble = (float)sinf(speed * 0.14) + sinf( speed * 0.013) + sinf(speed * 0.012);
	if (mode == 1) // helical, up and down flying motion
	{
		t_v3	snake_head	= {0.3, 0, 6};

		snake_head.x += sin(speed / 4) * 1.3 + wobble * 0.2;
		snake_head.z += cos(speed / 4) * 1.6 + wobble * 0.2;
		snake_head.y += cos(speed / 30) * 5 + wobble * 0.05;

		return (snake_head);
	}
	else // clockwise rotating flying motion
	{
		t_v3	snake_head	= {0.3, -0.5, 4.7};

		snake_head.x += sinf(speed / 10) * 1.6 + wobble * 0.2;
		snake_head.z += cosf(speed / 10) * 4 + wobble * 0.1;
		snake_head.y += cosf(speed / 5.3) * 0.2 + wobble * 0.05;

		return (snake_head);
	}
}

// muuta speedi timeks
// timen vois ottaa get_tickillä, ei tarvii deltaa
// muuta speediä sinillä
void three_d_rend(t_rend *rend, t_snake *snake)
{
	static float	speed;
	static int	mode;

	if(snake->k.u)
		mode = ~mode & 1;
	draw_snake(rend, snake_movement(speed, mode));	
	speed = SDL_GetTicks() * 0.01;
}

static void	loop(t_rend *rend, SDL_Event *e, t_snake *snake)
{
	bzero(rend->win_buffer->pixels, WIN_H * WIN_W * sizeof(uint32_t));
	keyevent(e, rend, snake);
	three_d_rend(rend, snake);
	draw_2_window(rend);
	fps_counter();
}


/////// testaa gprofilla
//		gprof käyttää -pg ja -g flageja, ota ne pois kun ei enää tarvita
int	main(void)
{
	t_rend		rend;
	SDL_Event	e;
	t_snake		snake;

	init(&rend, &snake);
	while (rend.run)
		loop(&rend, &e, &snake);
	cleanup(&rend);
	return (0);
}
