#define DIR_BOT 1
#define DIR_BOTL 2
#define DIR_BOTR 3
#define DIR_TOP 4
#define DIR_TOPL 5
#define DIR_TOPR 6
#define SIDE_LEFT 1
#define SIDE_TOP 2
#define SIDE_RIGHT 3
#define SIDE_BOT 4

typedef struct{
	int x;
	int y;
	int size;
	int speed;
	byte dir;
	byte side_reached;
} BALL, *BALLP;

typedef struct{
	int oldx;
	int oldy;
	int x;
	int y;
	int w;
	int h;
	int points;
} PALLETE, *PALLETEP;

static BALL ball={0};
static PALLETE player={0};
static int BORDER_THICKNESS=4;
static int PALETTE_H=16;
static byte main_loop=1;
int min_y=16;
byte game_running=0;
char *sb_points=NULL;
char *sb_speed=NULL;
char *sb_main=NULL;
//char *

LIBAROMA_CANVASP cv;

int abs_x_toplayer(int source){
	int res=source-(player.w/2);
	return res;
}

void player_setx(int x){
	player.oldx=player.x;
	player.x=x;
}

void sb_draw(){
	libaroma_draw_rect(cv, 0, 0, (cv->w), libaroma_dp(16), RGB(000000), 0xFF);
	libaroma_draw_text(cv, sb_speed, 0, 0, RGB(FFFFFF), (cv->w), LIBAROMA_TEXT_LEFT|LIBAROMA_TEXT_SINGLELINE, 0);
	libaroma_draw_text(cv, sb_points, 0, 0, RGB(FFFFFF), (cv->w), LIBAROMA_TEXT_RIGHT|LIBAROMA_TEXT_SINGLELINE, 0);
	libaroma_draw_text(cv, sb_main, 0, 0, RGB(FFFFFF), (cv->w), LIBAROMA_TEXT_CENTER|LIBAROMA_TEXT_SINGLELINE, 0);
}

void pallete_draw(){
	libaroma_draw_rect(cv, player.oldx, player.oldy, player.w, player.h, RGB(000000), 0xFF);
	libaroma_draw_rect(cv, player.x, player.y, player.w, player.h, RGB(FFFFFF), 0xFF);
}

void ball_update(){
	//first, check if ball reached any side
	if (ball.side_reached) ball.side_reached=0;
	if (ball.x < BORDER_THICKNESS)
		ball.side_reached=SIDE_LEFT;
	if (ball.y < min_y+BORDER_THICKNESS)
		ball.side_reached=SIDE_TOP;
	if (ball.x+ball.size > libaroma_fb()->w-BORDER_THICKNESS)
		ball.side_reached=SIDE_RIGHT;
	if (ball.y+ball.size > player.y){
		if (ball.x+ball.size>=player.x && ball.x+ball.size<=(player.x+player.w)){
			ball.side_reached=SIDE_BOT;
			player.points++;
			sprintf(sb_points, "Score: %d", player.points);
		}
	}
	if (ball.side_reached){
		switch (ball.dir){
			case DIR_TOP:
				ball.dir=DIR_BOT;
				break;
			case DIR_TOPL:
				if (ball.side_reached==SIDE_TOP)
					ball.dir=DIR_BOTL;
				else //if (ball.side_reached==SIDE_LEFT)
					ball.dir=DIR_TOPR;
				break;
			case DIR_TOPR:
				if (ball.side_reached==SIDE_TOP)
					ball.dir=DIR_BOTR;
				else
					ball.dir=DIR_TOPL;
				break;
			case DIR_BOT:
				ball.dir=DIR_TOP;
				break;
			case DIR_BOTL:
				if (ball.side_reached==SIDE_BOT)
					ball.dir=DIR_TOPL;
				else
					ball.dir=DIR_BOTR;
				break;
			case DIR_BOTR:
				if (ball.side_reached==SIDE_BOT)
					ball.dir=DIR_TOPR;
				else
					ball.dir=DIR_BOTL;
				break;
		}
	}
	//after that, clear the framebuffer at the ball location
	libaroma_draw_rect(cv, ball.x, ball.y, ball.size, ball.size, RGB(000000), 0xFF);
	//next, update the ball coords
	switch (ball.dir){
		case DIR_TOP:
			ball.y-=ball.speed;
			break;
		case DIR_TOPL:
			ball.x-=ball.speed;
			ball.y-=ball.speed;
			break;
		case DIR_TOPR:
			ball.x+=ball.speed;
			ball.y-=ball.speed;
			break;
		case DIR_BOT:
			ball.y+=ball.speed;
			break;
		case DIR_BOTL:
			ball.x-=ball.speed;
			ball.y+=ball.speed;
			break;
		case DIR_BOTR:
			ball.x+=ball.speed;
			ball.y+=ball.speed;
			break;
	}
	if (ball.y+ball.size > cv->h) game_end();
	//finally, draw the ball again
	libaroma_draw_circle(cv, RGB(FF0000), ball.x+(ball.size/2), ball.y+(ball.size/2), ball.size, 0xFF);
	//printf("Ball drawn at %d, %d\n", ball.x, ball.y);
}

byte game_loop(){
	libaroma_draw_rect(cv, 0, 0, cv->w, 16, RGB(00FF00), 0xFF);
	libaroma_draw_line(cv, 1, min_y+2, cv->w-1, min_y+2, 4.0, RGB(FFFFFF), 0xFF, 0); //top
	//libaroma_draw_line(cv, 1, cv->h-1, cv->w-1, cv->h-1, 4.0, RGB(FFFFFF), 0xFF, 0); //bot
	libaroma_draw_line(cv, 2, min_y+2, 2, cv->h, 4.0, RGB(FFFFFF), 0xFF, 0); //left
	libaroma_draw_line(cv, cv->w-1, min_y+2, cv->w-1, cv->h-1, 4.0, RGB(FFFFFF), 0xFF, 0); //right
	if (game_running){
		pallete_draw();
		ball_update();
	}
	sb_draw();
}

void game_end(){
	sprintf(sb_speed, "");
	sprintf(sb_points, "");
	sprintf(sb_main, "F, power para reiniciar");
	game_running=0;
}

void load_settings(){
	player.w=110;
	player.h=10;
	player.points=0;
	if (sb_speed) free(sb_speed);
	if (sb_points) free(sb_points);
	if (sb_main) free(sb_main);
	sb_speed=malloc(64);
	sb_points=malloc(96);
	sb_main=malloc(128);
	player.oldx=player.x=((cv->w/2)-(player.w/2));
	player.oldy=player.y=cv->h-libaroma_dp(40);
	ball.size=32;
	ball.x=(libaroma_fb()->w/2)-(ball.size/2);
	ball.y=(libaroma_fb()->h-libaroma_dp(80))-(ball.size/2);
	ball.dir=DIR_TOPL;
	ball.speed=1;
	ball.side_reached=0;
	sprintf(sb_speed, "Speed: %d", ball.speed);
	sprintf(sb_points, "Score: %d", player.points);
	sprintf(sb_main, "PONG");
}

void winmain(){
	printf("Starting PONG!\n");
	cv=libaroma_fb()->canvas;
	min_y=libaroma_dp(min_y);
	load_settings();
	game_end();
	sprintf(sb_main, "power to start, vol keys change speed");
	libaroma_canvas_blank(cv);
	libaroma_wm_set_ui_thread(game_loop);
	while (main_loop) {
		LIBAROMA_MSG msg;
		libaroma_msg(&msg);
		switch (msg.msg){
			case LIBAROMA_MSG_TOUCH:
				//printf("X: %d, Y: %d\n", msg.x, msg.y);
				if (msg.x >= player.x && msg.x <= (player.x+player.w))
					player_setx(abs_x_toplayer(msg.x));
				break;
			case LIBAROMA_MSG_KEY_SELECT:
				main_loop=0;
				break;
			case LIBAROMA_MSG_KEY_POWER:
				if (game_running)
					ball.speed=1;
				else {
					load_settings();
					libaroma_canvas_blank(cv);
					game_running=1;
				}
				break;
			case LIBAROMA_MSG_KEY_VOLUP:
				ball.speed++;
				sprintf(sb_speed, "Speed: %d", ball.speed);
				break;
			case LIBAROMA_MSG_KEY_VOLDOWN:
				if (ball.speed>0) {
					ball.speed--;
					sprintf(sb_speed, "Speed: %d", ball.speed);
				}
				break;
		}
		libaroma_sleep(16); 
	}
}