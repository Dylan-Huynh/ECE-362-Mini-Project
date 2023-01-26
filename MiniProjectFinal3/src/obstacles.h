#include <stdbool.h>

typedef struct _Obstacle {
	int y_pos;
	int x_pos;
	int y_vel;
	int x_vel;
	int width;
	int height;
	bool status;
} Obstacle;

Obstacle initalize_obstacle(Obstacle obstacle, float x_pos, float y_pos, float velo_x,
		float velo_y, int width, int height);

Obstacle update_obstacle_pos(Obstacle obstacle, float y_vel);

