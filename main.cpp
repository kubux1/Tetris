#define _USE_MATH_DEFINES
#define level_time 10000 //czas poziomu
#define level_amount 9 //ilosc poziomow w grze
#define instant_speed 950 // natychmiastowe opadanie klocka
#define Start_Pos 5 //pozycja startowa figur
#define fall_time 1000 //czas, od ktorego jest zalenza predkosc opadania klockow
#define level_speed_increase 175 //o ile zwiekszac szybkosc opadania co poziom
#define Blocks_SIZE 20 //dlugosc boku kwadratu klocka
#define map_hight 17
#define map_width 25

#include<math.h>
#include<stdio.h>
#include<string.h> 
#include <cstdlib>
#include <ctime>
extern "C" {
//#ifdef BIT64
//#include"./sdl64/include/SDL.h"
//#include"./sdl64/include/SDL_main.h"
//#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
//#endif
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define X 0
#define Y 1
enum move_name {
	Move_down = 1,
	Move_left = 2,
    Move_right = 3,
};
enum block_type {
	I,
    J,
	L,
	O,
	S,
	T,
	Z
}block;

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};
// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};
// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
 	*(Uint32 *)p = color;
	};
// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};
int check_col_rot(int block_type, int block_cords[4][2], int map_size[map_hight][map_width], int i, int z, int x, int c) { //sprawdzanie kolziji podczas obracania
	switch (block_type) {
	case I:
		if (map_size[block_cords[0][X] + i][block_cords[0][Y]] < 0 && map_size[block_cords[1][X] + (z + i)][block_cords[1][Y] + z] < 0 &&
			map_size[block_cords[2][X] - (z - i)][block_cords[2][Y] - z] < 0 && map_size[block_cords[3][X] - (2 * z - i)][block_cords[3][Y] - 2 * z] < 0)
			 {return 1;}		
		else return 0; break;
	case J:
		if (map_size[(block_cords[0][X] + i)][(block_cords[0][Y] + i)] < 0 && map_size[(block_cords[1][X] + z + i)][(block_cords[1][Y] + z+i)] < 0 &&
			map_size[(block_cords[2][X] - z)][(block_cords[2][Y] - (z + i))] < 0 && map_size[(block_cords[3][X] - (i + x))][(block_cords[3][Y] - (2 * z + i - x + c))] < 0		
			) {return 1;}		
		else return 0; break;

	case L:
		if (map_size[(block_cords[0][X] + ((-2 * z + x) + i) - c)][block_cords[0][Y] + (i + x)] < 0 && map_size[(block_cords[1][X] + (-z + i))][block_cords[1][Y] + (-z) + i] < 0 &&
			map_size[(block_cords[2][X] - i)][(block_cords[2][Y] - i)] < 0 && map_size[(block_cords[3][X] - (-z + i))][(block_cords[3][Y] - (-z + i))] < 0)
			 {return 1;}		
		else return 0;
		break;
	case S:
		if (map_size[(block_cords[0][X] + z)][block_cords[0][Y] + z] < 0 && map_size[(block_cords[1][X] + i)][block_cords[1][Y] + 2 * z] < 0 &&
			map_size[(block_cords[2][X] - i)][(block_cords[2][Y] - i)] < 0 && map_size[(block_cords[3][X] - (-z + i))][(block_cords[3][Y] - (-z + i))] < 0)
			 {return 1;}	
		else return 0; break;
	case Z:
		if (map_size[(block_cords[0][X] + z)][(block_cords[0][Y] + z)] < 0 && map_size[(block_cords[1][X] + (2 * z + i))][(block_cords[1][Y] + i)] < 0 &&
			map_size[(block_cords[2][X] - i)][(block_cords[2][Y] - i)] < 0 && map_size[(block_cords[3][X] - (z + i))][(block_cords[3][Y] - (-z + i))] < 0){		
			return 1;}	
		else return 0; break;
	case T:
		if (map_size[(block_cords[0][X] + i)][(block_cords[0][Y] + i)] < 0 && map_size[(block_cords[1][X] + z)][(block_cords[1][Y] - z)] < 0 &&
			map_size[(block_cords[2][X] - z)][(block_cords[2][Y] - (-z))] < 0 && map_size[(block_cords[3][X] - (-z + i) - (c / 2))][(block_cords[3][Y] - (-z + i + x - (c / 2)))] < 0) {
			return 1;}			
		else return 0; break;
	}
} 

void Init(int block_number, int block_cords[4][2]) { //inicjalizacja wspolrzednych kwadratow dla danej figury
	switch (block_number)
	{
	case I:
		block_cords[0][X] = 0+Start_Pos; block_cords[0][Y] = 0;
		block_cords[1][X] = 0+ Start_Pos; block_cords[1][Y] = -1;
		block_cords[2][X] = 0+ Start_Pos; block_cords[2][Y] = 1;		
		block_cords[3][X] = 0+ Start_Pos; block_cords[3][Y] = 2;
		break;
	case J:
		block_cords[0][X] = 0 + Start_Pos; block_cords[0][Y] = 0;
		block_cords[1][X] = 0 + Start_Pos; block_cords[1][Y] = -1;
		block_cords[2][X] = 0 + Start_Pos; block_cords[2][Y] = 1;
		block_cords[3][X] = -1 + Start_Pos; block_cords[3][Y] = 1;
		break;
	case L:
		block_cords[0][X] = 1 + Start_Pos; block_cords[0][Y] = 1;
		block_cords[1][X] = 0 + Start_Pos; block_cords[1][Y] =1;
		block_cords[2][X] = 0 + Start_Pos; block_cords[2][Y] = 0;
		block_cords[3][X] =0 + Start_Pos; block_cords[3][Y] =-1;
		break;
	case O:
		block_cords[0][X] = 0 + Start_Pos; block_cords[0][Y] = 0;
		block_cords[1][X] = 0 + Start_Pos; block_cords[1][Y] = -1;
		block_cords[2][X] = 1 + Start_Pos; block_cords[2][Y] = 0;
		block_cords[3][X] = 1 + Start_Pos; block_cords[3][Y] = -1;	
		break;
	case S:
		block_cords[0][X] = 1 + Start_Pos; block_cords[0][Y] = 1;
		block_cords[1][X] = 2 + Start_Pos; block_cords[1][Y] = 1;
		block_cords[2][X] = 1 + Start_Pos; block_cords[2][Y] = 2;
		block_cords[3][X] = 0 + Start_Pos; block_cords[3][Y] = 2;
		break;
	case T:
		block_cords[0][X] = 1 + Start_Pos; block_cords[0][Y] = 1;
		block_cords[1][X] = 0 + Start_Pos; block_cords[1][Y] = 1;
		block_cords[2][X] = 2 + Start_Pos; block_cords[2][Y] = 1;
		block_cords[3][X] = 1 + Start_Pos; block_cords[3][Y] = 0;
		break;
	case Z:
		block_cords[0][X] = 1 + Start_Pos; block_cords[0][Y] = 1;
		block_cords[1][X] = 0 + Start_Pos; block_cords[1][Y] = 1;
		block_cords[2][X] = 1 + Start_Pos; block_cords[2][Y] = 2;
		block_cords[3][X] = 2 + Start_Pos; block_cords[3][Y] = 2;
		break;
	default:
		break;
	}
} 
int rotate(int block_type, int block_cords[4][2],int map_size[map_hight][map_width],int degree){
    int i = 0, z = 0, x = 0, c = 0;
	if (degree == 0)z = -1,x=2,c=4;
	if( degree == 180)z = -1, x = -2;
	if (degree == 90 || degree == 270) {
		z = 1;
		if (degree == 270)
			c= -4;
		if (map_size[block_cords[3][X] - 1][block_cords[3][Y]] > -1) {
			i = 2;	
		}
		if (map_size[block_cords[1][X] + 1][block_cords[1][Y]] > -1) {
			i = -1;
		}
	}
	switch (block_type){
	case I:
		if (check_col_rot( block_type, block_cords, map_size,  i, z, x, c)) {
			block_cords[0][X] += i;
			block_cords[1][X] += (z + i); block_cords[1][Y] += z;
			block_cords[2][X] -= (z - i); block_cords[2][Y] -= z;
			block_cords[3][X] -= (2 * z - i); block_cords[3][Y] -= 2 * z;
			return degree;
		}
		else
			return (degree- 90);
		break;
	case J:
		if (check_col_rot(block_type, block_cords, map_size, i, z, x, c)) {
			block_cords[0][X] += (i); block_cords[0][Y] += i;
			block_cords[1][X] += (z + i); block_cords[1][Y] += (z)+i;
			block_cords[2][X] -= (z); block_cords[2][Y] -= z + i;
			block_cords[3][X] -= (i)+x; block_cords[3][Y] -= 2 * z + i - x + c;
			return degree;
		}
		else 
			return (degree - 90);		
		break;
	case L: 
		if (check_col_rot(block_type, block_cords, map_size, i, z, x, c)) {
			block_cords[0][X] += ((-2 * z + x) + i) - c; block_cords[0][Y] += (i + x);
			block_cords[1][X] += (-z + i); block_cords[1][Y] += (-z) + i;
			block_cords[2][X] -= (i); block_cords[2][Y] -= i;
			block_cords[3][X] -= -z + i; block_cords[3][Y] -= -z + i;
			return degree;
		}
		else
			return(degree - 90);
		break;
	case S: 
		if (check_col_rot(block_type, block_cords, map_size, i, z, x, c)) {
			block_cords[0][X] += z; block_cords[0][Y] += z;
			block_cords[1][X] += (i); block_cords[1][Y] += 2 * z;
			block_cords[2][X] -= (i); block_cords[2][Y] -= i;
			block_cords[3][X] -= -z + i; block_cords[3][Y] -= z + i;
			return degree;
		}
		else
			return (degree - 90);
		break;
	case T: 
		if (check_col_rot(block_type, block_cords, map_size, i, z, x, c)) {
			block_cords[0][X] += i; block_cords[0][Y] += i;
			block_cords[1][X] += z; block_cords[1][Y] += -z;
			block_cords[2][X] -= z; block_cords[2][Y] -= -z;
			block_cords[3][X] -= -z + i - (c / 2); block_cords[3][Y] -= -z + i + x - (c / 2);
			return degree;
		}
		else
			return(degree - 90);
		break;	
	case Z: 
		if (check_col_rot(block_type, block_cords, map_size, i, z, x, c)) {
			block_cords[0][X] += z; block_cords[0][Y] += z;
			block_cords[1][X] += (2 * z + i); block_cords[1][Y] += i;
			block_cords[2][X] -= (i); block_cords[2][Y] -= i;
			block_cords[3][X] -= z + i; block_cords[3][Y] -= -z + i;
			return degree;
		}
		else
			return(degree - 90);
		break;
	}
}
int count_lines(int map_size[map_hight][map_width], int multiplier) {
	int x;
	bool full_line;
	for (x = 21; x > 0; x--) {
		for (int i = 1; i < 13; i++) {
			if (map_size[i][x] > -1) {
				full_line = true;
			}
			else {
				full_line = false;
				break;
			}
		}
		if (full_line == true) {
			multiplier += 1;
		}
	}
	return multiplier;
}
void remove_lines(int map_size[map_hight][map_width]) {
	int x;
	bool full_line;
	for (x = 21; x > 0; x--) {
		for (int i = 1; i < 13; i++) {
			if (map_size[i][x] > -1) {
				full_line =true;
			}
			else {
				full_line = false;
				break;
			}
		}
			if (full_line == true) {
				for (x; x > 0; x--) {
					for (int y = 12; y > 0; y--) {
						if (map_size[y][x] > -1)
							if (map_size[y][x- 1] < -2 && map_size[y][x - 1] > 8)
								map_size[y][x - 1] = -1;
						map_size[y][x] = map_size[y][x - 1];
					}
				}
			}
		}
}
int count_points(int multiplier,int level,int points,int *doubl) {
	if (multiplier == 4) {
		*doubl += 1;
		if (*doubl > 2)
			*doubl = 1;
	}
	if (multiplier == 1) 
		points += 100 * (level + 1);
	if (multiplier == 2)
		points += 200 * (level + 1);
	if (multiplier == 3)
		points += 400 * (level + 1);
	if (multiplier == 4 && *doubl==1)
		points += 800 * (level + 1);
	if (multiplier == 4 && *doubl == 2)
		points += 1200 * (level + 1);
	return points;
}
//move
		void fall(int block_cords[4][2]) {
			for (int i = 0; i < 4; i++) {
				block_cords[i][1]++;
			}
		}
		void aside_left(int block_cords[4][2]) {
			for (int i = 0; i < 4; i++) {
				block_cords[i][0]--;
			}
		}
		void aside_right(int block_cords[4][2]) {
			for (int i = 0; i < 4; i++) {
				block_cords[i][0]++;
			}
		}
// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};
void DrawBlocks(SDL_Surface *screen, int block_cords[4][2], int map_size[map_hight][map_width], int block_number,int smooth_fall, int smooth_push) { 
	int color;
	int green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int aqua = SDL_MapRGB(screen->format, 0x30, 0xD5, 0xC8);
	int yellow = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x00);
	int violet = SDL_MapRGB(screen->format, 0xBF, 0x00, 0xFF);
	int orange = SDL_MapRGB(screen->format, 0xFF, 0x90, 0x00);
	switch (block_number)
	{
	case I:
		color = aqua;
		break;
	case J:
		color = orange;
		break;
	case L:
		color = red;
		break;
	case O:
		color = yellow;
		break;
	case S:
		color = blue;
		break;
	case T:
		color = violet;
		break;
	case Z:
		color = green;
		break;
	default:
		break;
	}	
		DrawRectangle(screen, block_cords[0][X] * Blocks_SIZE+ smooth_push, block_cords[0][Y] * Blocks_SIZE + (smooth_fall), Blocks_SIZE, Blocks_SIZE, NULL, color);
		DrawRectangle(screen, (block_cords[1][X] * Blocks_SIZE)+ smooth_push, block_cords[1][Y] * Blocks_SIZE + (smooth_fall), Blocks_SIZE, Blocks_SIZE, NULL, color);
		DrawRectangle(screen, (block_cords[2][X] * Blocks_SIZE)+ smooth_push, block_cords[2][Y] * Blocks_SIZE + (smooth_fall), Blocks_SIZE, Blocks_SIZE, NULL, color);
		DrawRectangle(screen, block_cords[3][X] * Blocks_SIZE+ smooth_push, block_cords[3][Y] * Blocks_SIZE + (smooth_fall), Blocks_SIZE, Blocks_SIZE, NULL, color);
}

void Map(SDL_Surface *screen, int map_size1[map_hight][map_width], int block_number) { // draw blocks on the map 
	int color;
	int green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int aqua = SDL_MapRGB(screen->format, 0x30, 0xD5, 0xC8);
	int yellow = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x00);
	int violet = SDL_MapRGB(screen->format, 0xBF, 0x00, 0xFF);
	int orange = SDL_MapRGB(screen->format, 0xFF, 0x90, 0x00);
	switch (block_number)
	{
	case I:
		color = aqua;
		break;
	case J:
		color = orange;
		break;
	case L:
		color = red;
		break;
	case O:
		color = yellow;
		break;
	case S:
		color = blue;
		break;
	case T:
		color = violet;
		break;
	case Z:
		color = green;
		break;
	default:
		break;
	}
	for (int y = 1; y < 13; y++) {
		for (int x = 0; x < 22; x++) {
			if (map_size1[y][x]==0)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, aqua);
			if (map_size1[y][x] == 1)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL,orange);
			if (map_size1[y][x] == 2)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, red);
			if (map_size1[y][x] == 3)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, yellow);
			if (map_size1[y][x] == 4)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, blue);
			if (map_size1[y][x] ==5)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, violet);
			if (map_size1[y][x] == 6)
				DrawRectangle(screen, (y)* 20, (x)* 20, Blocks_SIZE, Blocks_SIZE, NULL, green);
		
	}}
}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	srand(time(NULL));
	int t1, t2, quit=0, rc, map_size[map_hight][map_width], map_size1[map_hight][map_width], block_cords[7][2], inst_fall = 0, degree = 0, level = 0, points = 0;
	int multiplier = 0, doubl = 0, count = 0, action = Move_down, smooth_push = 0, block_number = rand() % 7, smooth_fall = 0,speed_fall=0;
	double delta, worldTime = 0;
	bool pause = false, end =false;
	char text[128];
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}
	for (int z = 1; z <( map_hight-1); z++) {
		for (int i = 0; i <(map_width-1); i++) {
			map_size[13][i] = 7;
			map_size[14][i] = 7;
			map_size[15][i] = 7;
			map_size[0][i] = 7;
			map_size[z][22] = 7;
			map_size[z][23] = 7;
		}
	}
	Init(block_number, block_cords);
	// tryb pe³noekranowy
    //rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
                   //            &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Tetris - Jakub Kotowski 161969");
	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);
	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);
	
	eti = SDL_LoadBMP("./interface.bmp");
	if(eti == NULL) {
		printf("SDL_LoadBMP(Box.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	t1 = SDL_GetTicks();
	fall(block_cords);
	while (!quit) {
		if (end == false) {
			if (pause == false) {
				if (map_size[block_cords[0][0]][block_cords[0][1] + 1] > -1 || map_size[block_cords[1][0]][block_cords[1][1] + 1] > -1 ||
					map_size[block_cords[2][0]][block_cords[2][1] + 1] > -1 || map_size[block_cords[3][0]][block_cords[3][1] + 1] > -1) {

					for (int i = 0; i < 4; i++) {
						map_size[block_cords[i][0]][block_cords[i][1]] = block_number;
					}
					block_number = rand() % 7;
					Init(block_number, block_cords);
					fall(block_cords);
					degree = 0;
					inst_fall = 0;

				}
				t2 = SDL_GetTicks();
				// w tym momencie t2-t1 to czas w milisekundach,
				// jaki uplyna³ od ostatniego narysowania ekranu
				// delta to ten sam czas w sekundach
				delta = (t2 - t1);
				//t1 = t2;
		//	if((fall_time - delta - speed_fall - inst_fall )<0.1){

				if (double((fall_time - speed_fall - inst_fall) / Blocks_SIZE) - delta  < 1) {
					t1 = t2;
					if (action == Move_down)
						smooth_fall += 1;
					if (smooth_fall == Blocks_SIZE) {
						smooth_fall = 0;
						if (map_size[block_cords[0][0]][block_cords[0][1] + 1] > -1 || map_size[block_cords[1][0]][block_cords[1][1] + 1] > -1 || //sprawdzanie kolizji 
							map_size[block_cords[2][0]][block_cords[2][1] + 1] > -1 || map_size[block_cords[3][0]][block_cords[3][1] + 1] > -1) {
							action = 0;
						}
						fall(block_cords);

					}
					if (action == Move_left) {
						smooth_push -= 1;
						if (smooth_push == -Blocks_SIZE) {
							aside_left(block_cords);
							smooth_push = 0;
							action = Move_down;
						}
					}
					if (action == Move_right) {
						smooth_push += 1;
						if (smooth_push == Blocks_SIZE) {
							aside_right(block_cords);
							smooth_push = 0;
							action = Move_down;
						}
					}

				}
				if (worldTime > level_time && level < level_amount) {
					worldTime = 0;
					speed_fall += level_speed_increase;
					level += 1;
				}
				worldTime += delta;
				SDL_FillRect(screen, NULL, czarny);

				//horizontal interface
				for (int i = 0; i < 640; i += 20) {
					DrawSurface(screen, eti, i + 10, 470);
					DrawSurface(screen, eti, i + 10, 450);
				}
				//vertical interface
				for (int y = 0; y <= 360; y += 20) {
					for (int i = 0; i < 480; i += 20) {
						DrawSurface(screen, eti, 10, i + 10);
						DrawSurface(screen, eti, 270, i + 10);
					}
				}

				// naniesienie wyniku rysowania na rzeczywisty ekran
		//		SDL_Flip(screen);
				DrawBlocks(screen, block_cords, map_size, block_number, smooth_fall, smooth_push);
				Map(screen, map_size, block_number);

				//tekst informacyjny
				for (int i = 1; i < 13; i++) {
					if (map_size[i][1]>-1) {
						end = true;
						sprintf(text, "KONIEC GRY. NACISNIJ N ABY ROZPOCZAC NOWA GRE ALBO ESC ABY WYJSC Z GRY");
						DrawString(screen, 10, 200, text, charset);
					}
				}
				//sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf ", (worldTime));
			//	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

				sprintf(text, "Level = %d  ", level);
				DrawString(screen, 300, 30, text, charset);
				sprintf(text, "Points = %d ", points);
				DrawString(screen, 300, 50, text, charset);
				sprintf(text, "P - pause");
				DrawString(screen, 300, 70, text, charset);
				sprintf(text, "\030 - rotate \031 - instant fall block");
				DrawString(screen, 300, 85, text, charset);
				sprintf(text, "S - increase level");
				DrawString(screen, 300, 100, text, charset);
				sprintf(text, "N - new game");
				DrawString(screen, 300, 115, text, charset);
				sprintf(text, "Esc - exit");
				DrawString(screen, 300, 130, text, charset);

				points = count_points(count_lines(map_size, multiplier), level, points, &doubl);
				remove_lines(map_size);
				remove_lines(map_size);
				remove_lines(map_size);
				remove_lines(map_size);
			}

			if (pause == true) {

				sprintf(text, "PAUSED");
				DrawString(screen, 300, 10, text, charset);

			}
}
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// obs³uga zdarzeñ (o ile jakieœ zasz³y)
		

		while(SDL_PollEvent(&event)&&inst_fall != instant_speed) {
					switch (event.type) {
					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_DOWN)	inst_fall = instant_speed;
						if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
							else if (event.key.keysym.sym == SDLK_UP) {
								degree += 90; if (degree == 360)degree = 0;  degree = rotate(block_number, block_cords, map_size, degree);
							}
							else if (event.key.keysym.sym == SDLK_n) {
								for (int z = 1; z < 13; z++) {
									for (int i = 0; i < 22; i++) {
										map_size[z][i] = -1;
									}
								};
								block_number = rand() % 7;
								Init(block_number, block_cords);
								fall(block_cords);
								points = 0;
								degree = 0;
								speed_fall = 0;
								level = 0;
								worldTime = 0;
								action = Move_down;
								worldTime = 0;
								end = false;
							}
							else if (event.key.keysym.sym == SDLK_s) {
								if (level<level_amount){
									level += 1;
									speed_fall += level_speed_increase;
								}
							}
							else if (event.key.keysym.sym == SDLK_p) {
								count += 1;
								if(count==1)
								pause = true;
								else {
									pause = false;
									count = 0;
								}
							}
							else if (event.key.keysym.sym == SDLK_LEFT) {
								if (map_size[block_cords[0][0] - 1][block_cords[0][1]] < 0)
									if (map_size[block_cords[1][0] - 1][block_cords[1][1]] < 0)
										if (map_size[block_cords[2][0] - 1][block_cords[2][1]] < 0)
											if (map_size[block_cords[3][0] - 1][block_cords[3][1]] < 0)
												//aside_left(block_cords);
								action = Move_left;
							}
							else if (event.key.keysym.sym == SDLK_RIGHT) {
								if (map_size[block_cords[0][0] + 1][block_cords[0][1]] < 0)
									if (map_size[block_cords[1][0] + 1][block_cords[1][1]] < 0)
										if (map_size[block_cords[2][0] + 1][block_cords[2][1]] < 0)
											if (map_size[block_cords[3][0] + 1][block_cords[3][1]] < 0)
												//aside_right(block_cords);
								action = Move_right;

							}
							break;

					case SDL_QUIT:
						quit = 1;
						break;
					};				
			};
		};

	// zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
