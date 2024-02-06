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

//using namespace cv;
//using namespace std;

const bool mazeMatrix[21][21] = {
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
		        maze.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255); // White
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
	while(maze.at<cv::Vec3b>(local_x, local_y) != cv::Vec3b(255, 255, 255)); 
	// this checks if the pixel is white or not, which avoids edge case of generating object on the same pixel
	
	maze.at<cv::Vec3b>(local_x, local_y) = cv::Vec3b(r, g, b); 
	*target_x = local_x;
	*target_y = local_y;
}

int move_pacman()
{
	// Wait for a key press
	int key = cv::waitKey(0);
	
	// Reset the old pixel to white
	maze.at<cv::Vec3b>(x, y) = cv::Vec3b(255, 255, 255); 

	// Check the pressed key
	switch (key) {
	case 27: // ESC key
		return 0;

	case 82: // Up arrow key
		if(x > 0 && maze.at<cv::Vec3b>(x-1, y) == cv::Vec3b(255, 255, 255)) x--;
		else if(x == 0 && maze.at<cv::Vec3b>(maze.rows-1, y) == cv::Vec3b(255, 255, 255)) x = maze.rows-1;
	break;

	case 84: // Down arrow key
		if(x < (maze.rows-1) && maze.at<cv::Vec3b>(x+1, y) == cv::Vec3b(255, 255, 255)) x++;
		else if(x == (maze.rows-1) && maze.at<cv::Vec3b>(0, y) == cv::Vec3b(255, 255, 255)) x = 0;
	break;

	case 81: // Left arrow key
		if (y > 0  && maze.at<cv::Vec3b>(x, y-1) == cv::Vec3b(255, 255, 255)) y--;
		else if(y == 0 && maze.at<cv::Vec3b>(x, maze.cols-1) == cv::Vec3b(255, 255, 255)) y = maze.cols-1;
	break;

	case 83: // Right arrow key
		if (y < (maze.cols-1)  && maze.at<cv::Vec3b>(x, y+1) == cv::Vec3b(255, 255, 255)) y++;
		else if(y == (maze.cols-1) && maze.at<cv::Vec3b>(x, 0) == cv::Vec3b(255, 255, 255)) y = 0;
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
		maze.at<cv::Vec3b>(x_ghost[i], y_ghost[i]) = cv::Vec3b(255, 255, 255); 
		
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
		cv::imshow("pacman", maze);	
		move_ghosts();	
		if(!check_game()) break;
	}
	while(move_pacman());
	
	if(game_over)
	{
		std::string text = "GAME";
		cv::putText(maze, text, cv::Point(1,10), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1, false);
		text = "OVER";
		cv::putText(maze, text, cv::Point(1,18), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1, false);
		cv::imshow("pacman", maze);
		
		while(1)
		{
			int key = cv::waitKey(0);
			if(key == 27 || cv::getWindowProperty("pacman", cv::WND_PROP_AUTOSIZE) == -1) break;
		}
	}
	

    return 0;
}
