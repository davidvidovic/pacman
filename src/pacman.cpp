#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

// For rand and srand
#include <cstdlib> 

// For time to setup random seed
#include <ctime> 

// For usleep
#include <unistd.h>

// For OMP
#include <omp.h>

//using namespace cv;
//using namespace std;

//#define RANDOM_GHOSTS

#define MAZE_SIZE	21


int mazeMatrix[MAZE_SIZE][MAZE_SIZE] = {
	{0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0},
	{0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0},
	{0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0},
	{0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0},
	{0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0},
	{0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0},
	{0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0},
	{0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
	{0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0},
	{0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0},
	{1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1},
	{0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0},
	{0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0},
	{0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0},
	{0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0},
	{0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0},
	{0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0},
	{1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
	{0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0}
};

// Position of pacman
int x, y;

// Game's maze
cv::Mat maze(21, 21, CV_8UC3, cv::Scalar(0,0,0));

// Goal holds number of gray pixels that pacman can eat
// When pacman eats all gray pixels (reaches the goal), game ends in a win
int goal = 0;

// Number of ghosts
int num_of_ghosts = 3;
int *x_ghost, *y_ghost;
int *ghost_dir;

// Var to keep score of game
int game_over = 0;



void create_map()
{
	for (int i = 0; i < 21; ++i) {
		for (int j = 0; j < 21; ++j) {
		    if (mazeMatrix[i][j] == 1) {
		        maze.at<cv::Vec3b>(i, j) =  cv::Vec3b(160, 160, 160); // Gray
		        
		        // Count all gray pixels
		        goal++;
		    }
		}
	}
}



void create_object(int *target_x, int *target_y, int r, int g, int b)
{
	int local_x, local_y;
	do
	{
		local_x = std::rand() % 21;
		local_y = std::rand() % 21;
	}
	while(maze.at<cv::Vec3b>(local_x, local_y) != cv::Vec3b(160, 160, 160)); 
	// this checks if the pixel is white or not, which avoids edge case of generating object on the same pixel
	
	maze.at<cv::Vec3b>(local_x, local_y) = cv::Vec3b(r, g, b); 
	*target_x = local_x;
	*target_y = local_y;
}


int move_pacman()
{
	// Wait for a key press
	int key = cv::waitKey(300);
	
	// Update mazeMatrix so that everyone (ghosts) knows pacman visited this pixel
	if(mazeMatrix[x][y] == 1)
	{
		#pragma omp critical // I think not necesseary because ghosts dont care about 1 or -1
		{
			mazeMatrix[x][y] = -1;
		}
		goal--;
	}

	// Check the pressed key
	switch (key) {
	case 27: // ESC key
		return 0;
	
	case 'w':
	case 'W':
	case 82: // Up arrow key
		if(x > 0 && mazeMatrix[x-1][y] != 0) x--;
		else if(x == 0 && mazeMatrix[MAZE_SIZE-1][y] != 0) x = MAZE_SIZE-1; // Make pacman loop from top to bottom
	break;

	case 's':
	//case 'S':
	case 84: // Down arrow key
		if(x < MAZE_SIZE-1 && mazeMatrix[x+1][y] != 0) x++;
		else if(x == MAZE_SIZE-1 && mazeMatrix[0][y] != 0) x = 0;
	break;

	case 'a':
	case 'A':
	case 81: // Left arrow key
		if (y > 0  && mazeMatrix[x][y-1] != 0) y--;
		else if(y == 0 && mazeMatrix[x][MAZE_SIZE-1] != 0) y = MAZE_SIZE-1;
	break;

	case 'd':
	case 'D':
	case 83: // Right arrow key
		if (y < MAZE_SIZE-1  && mazeMatrix[x][y+1] != 0) y++;
		else if(y == MAZE_SIZE-1 && mazeMatrix[x][0] != 0) y = 0;
	break;

	default:
		// Check if the window is closed by X icon
		if(cv::getWindowProperty("pacman", cv::WND_PROP_AUTOSIZE) == -1) 
		{
			#pragma omp critical
			{
				game_over = 1;
			}
			return 0;
		}
	break;
	}

	return 1;
}


void move_ghosts()
{
	for(int i = 0; i < num_of_ghosts; i++)
	{	
		switch(ghost_dir[i])
		{
		case 0: // UP
			if(x_ghost[i] > 0 && mazeMatrix[x_ghost[i]-1][y_ghost[i]] != 0) x_ghost[i]--; // Move ghost up one pixel
			else if(x_ghost[i] == 0 && mazeMatrix[MAZE_SIZE-1][y_ghost[i]] != 0) x_ghost[i] = MAZE_SIZE-1; // Loop ghost from top to bottom
			
			#ifdef RANDOM_GHOSTS
			else ghost_dir[i] = 1 + rand() % 3;
			#else
			// if none is available, find new direction based on pixels around you
			else if(mazeMatrix[x_ghost[i]][y_ghost[i]-1] != 0)  ghost_dir[i] = 3; // ghost can move left
			else if(mazeMatrix[x_ghost[i]][y_ghost[i]+1] != 0)  ghost_dir[i] = 1; // ghost can move right
			else ghost_dir[i] = 2;
			#endif
		break;
		
		case 1: // RIGHT
			if(y_ghost[i] < MAZE_SIZE-1 && mazeMatrix[x_ghost[i]][y_ghost[i]+1] != 0) y_ghost[i]++;
			else if(y_ghost[i] == MAZE_SIZE-1 && mazeMatrix[x_ghost[i]][0] != 0) y_ghost[i] = 0;
			
			#ifdef RANDOM_GHOSTS
			else ghost_dir[i] = rand() % 4;
			#else
			else if(mazeMatrix[x_ghost[i]-1][y_ghost[i]] != 0)  ghost_dir[i] = 0; // ghost can move up
			else if(mazeMatrix[x_ghost[i]+1][y_ghost[i]] != 0)  ghost_dir[i] = 2;
			else ghost_dir[i] = 3;
			#endif
		break;
		
		case 2: // DOWN
			if(x_ghost[i] < MAZE_SIZE-1 && mazeMatrix[x_ghost[i]+1][y_ghost[i]] != 0) x_ghost[i]++;
			else if(x_ghost[i] == MAZE_SIZE-1 && mazeMatrix[0][y_ghost[i]] != 0) x_ghost[i] = 0;
			
			#ifdef RANDOM_GHOSTS
			else ghost_dir[i] = rand() % 4;
			#else
			else if(mazeMatrix[x_ghost[i]][y_ghost[i]-1] != 0)  ghost_dir[i] = 3; // ghost can move left
			else if(mazeMatrix[x_ghost[i]][y_ghost[i]+1] != 0)  ghost_dir[i] = 1; // ghost can move right
			else ghost_dir[i] = 0;
			#endif
		break;
		
		case 3: // LEFT
			if(y_ghost[i] > 0 && mazeMatrix[x_ghost[i]][y_ghost[i]-1] != 0) y_ghost[i]--;
			else if(y_ghost[i] == 0 && mazeMatrix[x_ghost[i]][MAZE_SIZE-1] != 0) y_ghost[i] = MAZE_SIZE-1;
			
			#ifdef RANDOM_GHOSTS
			else ghost_dir[i] = rand() % 3;
			#else
			else if(mazeMatrix[x_ghost[i]-1][y_ghost[i]] != 0)  ghost_dir[i] = 0; // ghost can move up
			else if(mazeMatrix[x_ghost[i]+1][y_ghost[i]] != 0)  ghost_dir[i] = 2;
			else ghost_dir[i] = 1;
			#endif
		break;
		
		default:
			ghost_dir[i] = 0;
		break;
		}
		
	}
}


void update_maze()
{
	for (int i = 0; i < 21; ++i) {
		for (int j = 0; j < 21; ++j) {
		    if (mazeMatrix[i][j] == 1) {
		        maze.at<cv::Vec3b>(i, j) = cv::Vec3b(160, 160, 160); // Gray
		    }
		    
		    if (mazeMatrix[i][j] == -1) {
		        maze.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255); // White
		    }
		}
	}
	
	// Draw pacman
	maze.at<cv::Vec3b>(x, y) = cv::Vec3b(255, 0, 0); 
	
	// Draw ghosts
	for(int i =0; i < num_of_ghosts; i++)
	{
		maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]) = cv::Vec3b(255, 0, 255); 
	}
	
	cv::imshow("pacman", maze);	
}


int check_game()
{
	//std::cout << "MOVE\n";
	//std::cout << "x= " << x << " y= " << y << std::endl;
	
	int local_gm = 0;
	
	if(goal == 0)
	{
		local_gm = 2; // Code for win is 2
		//return 0;
	}
	
	for(int i = 0; i < num_of_ghosts; i++)
	{
	//	std::cout << "Ghost " << i << ": x= " << x_ghost[i] << " y= " << y_ghost[i] << std::endl;
		if(x_ghost[i] == x && y_ghost[i] == y) 
		{
			local_gm = 1;
			//return 0;
		}
	}
	
	//std::cout<< std::endl;
	
	#pragma omp critical
	{
		game_over = local_gm;
	}
	return local_gm;
}



int main(int argc, char** argv )
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	// Create map out of mazeMatrix
	create_map();
		
	// Create pacman at random spot, mark it as blue
	create_object(&x, &y, 255, 0, 0);
	
	// Create ghosts
	//num_of_ghosts = atoi(argv[1]);
	x_ghost = new int[num_of_ghosts];
	y_ghost = new int[num_of_ghosts];
	ghost_dir = new int[num_of_ghosts];
	
	for(int i = 0; i < num_of_ghosts; i++)
	{
		create_object(&x_ghost[i], &y_ghost[i], 255, 0, 255);
		ghost_dir[i] = std::rand() % 4;
	}
	
	// Create window for game
	cv::namedWindow("pacman", cv::WINDOW_NORMAL);
	cv::resizeWindow("pacman", 1080, 720); // set to 1080x720 HD
	
	// Game
	
	do
	{	
		if(check_game()) break;
		update_maze();
		move_ghosts();	
	}
	while(move_pacman());
	
	/*
	# pragma omp parallel sections num_threads(4) shared(maze, mazeMatrix, game_over)
	{
		#pragma omp section
		{
			while(!game_over)
			{
				#pragma omp critical
				{
					cv::imshow("pacman", maze);	
				}
			}
		}
		
		#pragma omp section
		{
			while(!check_game());
		}
		
		
		#pragma omp section
		{
			while(!game_over)
			{
				move_ghosts();	
				if(!move_pacman()) break;
			}
		}
	}
	*/
	
	// Game is over, pacman was eaten
	// Display GAME OVER and wait for key ESC or to shut down window
	
	
	if(game_over)
	{
		if(game_over == 1)
		{
			cv::Mat gameover_mat = cv::imread("./include/gameover.jpg", cv::IMREAD_COLOR);
			cv::imshow("pacman", gameover_mat);
		}
		else if(game_over == 2)
		{
			cv::Mat win_mat = cv::imread("./include/win.png", cv::IMREAD_COLOR);
			cv::imshow("pacman", win_mat);
		}
	
		while(1)
		{
			int key = cv::waitKey(0);
			if(key == 27 || cv::getWindowProperty("pacman", cv::WND_PROP_AUTOSIZE) == -1) break;
		}
	}
	

    return 0;
}
