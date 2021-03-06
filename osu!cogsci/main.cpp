#include "game.h"
#undef main

int main(int argc, const char* argv[]) {
	Game game; //Remember that the default constructor starts the game loop

	return 0;
}

/*

LINKER GUIDE - IMPORTANT!

1. Open Debug->[project_name] Properties...

2. Navigate to C/C++->General
Under "Additional Include Directories", type "SDL_include"

3. Navigate to Linker->General
Under "Additional Library Directories", type "SDL_lib_x64;SDL_Image_lib_x64;%(AdditionalLibraryDirectories)"

4. Navigate to Linker->Input
Under "Additional Dependencies", type "SDL2.lib;SDL2main.lib;SDL2_image.lib;%(AdditionalDependencies)"

5. Hit "Apply" and it should show up as a blank black screen. Exit by closing the terminal.

6. Build your project!

*/