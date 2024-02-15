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


int mazeMatrix[21][21] = {
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
unsigned char game_over = 0;



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
	
	// Reset the old pixel to white
	maze.at<cv::Vec3b>(x, y) = cv::Vec3b(255, 255, 255); 
	
	// Update mazeMatrix so that everyone (ghosts) knows pacman visited this pixel
	if(mazeMatrix[x][y] == 1)
	{
		mazeMatrix[x][y] = -1;
		goal--;
	}

	// Check the pressed key
	switch (key) {
	case 27: // ESC key
		return 0;
	
	case 'w':
	case 'W':
	case 82: // Up arrow key
		if(x > 0 && maze.at<cv::Vec3b>(x-1, y) != cv::Vec3b(0, 0, 0)) x--;
		else if(x == 0 && maze.at<cv::Vec3b>(maze.rows-1, y) != cv::Vec3b(0, 0, 0)) x = maze.rows-1;
	break;

	case 's':
	//case 'S':
	case 84: // Down arrow key
		if(x < (maze.rows-1) && maze.at<cv::Vec3b>(x+1, y) != cv::Vec3b(0, 0, 0)) x++;
		else if(x == (maze.rows-1) && maze.at<cv::Vec3b>(0, y) != cv::Vec3b(0, 0, 0)) x = 0;
	break;

	case 'a':
	case 'A':
	case 81: // Left arrow key
		if (y > 0  && maze.at<cv::Vec3b>(x, y-1) != cv::Vec3b(0, 0, 0)) y--;
		else if(y == 0 && maze.at<cv::Vec3b>(x, maze.cols-1) != cv::Vec3b(0, 0, 0)) y = maze.cols-1;
	break;

	case 'd':
	case 'D':
	case 83: // Right arrow key
		if (y < (maze.cols-1)  && maze.at<cv::Vec3b>(x, y+1) != cv::Vec3b(0, 0, 0)) y++;
		else if(y == (maze.cols-1) && maze.at<cv::Vec3b>(x, 0) != cv::Vec3b(0, 0, 0)) y = 0;
	break;

	default:
		// Check if the window is closed by X icon
		if(cv::getWindowProperty("pacman", cv::WND_PROP_AUTOSIZE) == -1) 
			return 0;
	break;
	}
	
	maze.at<cv::Vec3b>(x, y) = cv::Vec3b(255, 0, 0); 
	return 1;
}


void move_ghosts()
{
	for(int i = 0; i < num_of_ghosts; i++)
	{
		// Reset the old pixel to white
		if(mazeMatrix[x_ghost[i]][y_ghost[i]] == 1) maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]) = cv::Vec3b(160, 160, 160); // Gray pixel stays gray
		else maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]) = cv::Vec3b(255, 255, 255); // White pixel
		
		switch(ghost_dir[i])
		{
		case 0: // UP
			if(x_ghost[i] > 0 && maze.at<cv::Vec3b>(x_ghost[i]-1, y_ghost[i]) != cv::Vec3b(0, 0, 0)) x_ghost[i]--;
			else if(x_ghost[i] == 0 && maze.at<cv::Vec3b>(maze.rows-1, y_ghost[i]) != cv::Vec3b(0, 0, 0)) x_ghost[i] = maze.rows-1;
			// if none is available, find new direction based on pixels around you
			else if(maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]-1) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 3; // ghost can move left
			else if(maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]+1) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 1;
			else ghost_dir[i] = 2;
		break;
		
		case 1: // RIGHT
			if(y_ghost[i] < (maze.cols-1) && maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]+1) != cv::Vec3b(0, 0, 0)) y_ghost[i]++;
			else if(y_ghost[i] == (maze.cols-1) && maze.at<cv::Vec3b>(x_ghost[i], 0) != cv::Vec3b(0, 0, 0)) y_ghost[i] = 0;
			
			else if(maze.at<cv::Vec3b>(x_ghost[i]-1, y_ghost[i]) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 0; // ghost can move up
			else if(maze.at<cv::Vec3b>(x_ghost[i]+1, y_ghost[i]) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 2;
			else ghost_dir[i] = 3;
		break;
		
		case 2: // DOWN
			if(x_ghost[i] < (maze.rows-1) && maze.at<cv::Vec3b>(x_ghost[i]+1, y_ghost[i]) != cv::Vec3b(0, 0, 0)) x_ghost[i]++;
			else if(x_ghost[i] == (maze.rows-1) && maze.at<cv::Vec3b>(0, y_ghost[i]) != cv::Vec3b(0, 0, 0)) x_ghost[i] = 0;
			
			else if(maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]-1) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 3; // ghost can move left
			else if(maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]+1) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 1;
			else ghost_dir[i] = 0;
		break;
		
		case 3: // LEFT
			if(y_ghost[i] > 0 && maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]-1) != cv::Vec3b(0, 0, 0)) y_ghost[i]--;
			else if(y_ghost[i] == 0 && maze.at<cv::Vec3b>(x_ghost[i], maze.cols-1) != cv::Vec3b(0, 0, 0)) y_ghost[i] = maze.cols-1;
			
			else if(maze.at<cv::Vec3b>(x_ghost[i]-1, y_ghost[i]) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 0; // ghost can move up
			else if(maze.at<cv::Vec3b>(x_ghost[i]+1, y_ghost[i]) != cv::Vec3b(0, 0, 0))  ghost_dir[i] = 2;
			else ghost_dir[i] = 1;
		break;
		
		default:
			ghost_dir[i] = 0;
		break;
		}
		
		maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]) = cv::Vec3b(255, 0, 255); 
	}
}

int check_game()
{
	//std::cout << "MOVE\n";
	//std::cout << "x= " << x << " y= " << y << std::endl;
	
	if(goal == 0)
	{
		game_over = 2; // Code for win is 2
		return 0;
	}
	
	for(int i = 0; i < num_of_ghosts; i++)
	{
	//	std::cout << "Ghost " << i << ": x= " << x_ghost[i] << " y= " << y_ghost[i] << std::endl;
		if(x_ghost[i] == x && y_ghost[i] == y) 
		{
			game_over = 1;
			return 0;
		}
	}
	
	//std::cout<< std::endl;
	
	return 1;
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
		if(!check_game()) break;
		cv::imshow("pacman", maze);	
		move_ghosts();	
	}
	while(move_pacman());
	
	/*
	# pragma omp parallel num_threads(2)
	{
		while(check_game())
		{
			//std::cout << omp_get_thread_num() << std::endl;	
			//int key = cv::waitKey(300);
			if(omp_get_thread_num() == 0) 
			{	
				int key1= cv::waitKey(100);
				cv::imshow("pacman", maze);	
				move_ghosts();		
				//std::cout << omp_get_thread_num() << std::endl;		
			}
			else if(omp_get_thread_num() == 1) 
			{
				int key = cv::waitKey(0);
				move_pacman(key); 
				//std::cout << omp_get_thread_num() << std::endl;	
			}
		}
	}
	*/
	
	// Game is over, pacman was eaten
	// Display GAME OVER and wait for key ESC or to shut down window
	if(game_over == 1)
	{
		cv::Mat gameover_mat = cv::imread("./include/gameover.jpg", cv::IMREAD_COLOR);
		cv::imshow("pacman", gameover_mat);
	}
	else
	{
		cv::Mat win_mat = cv::imread("./include/win.png", cv::IMREAD_COLOR);
		cv::imshow("pacman", win_mat);
	}
	
	while(1)
	{
		int key = cv::waitKey(0);
		if(key == 27 || cv::getWindowProperty("pacman", cv::WND_PROP_AUTOSIZE) == -1) break;
	}
	

    return 0;
}
