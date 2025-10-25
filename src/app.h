#ifndef APP_H
#define APP_H

#include <stdbool.h>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

#define BG_GREEN     "\033[42m"

typedef struct {
    float weight;
    float daily_goal;
    float consumed;
    int goal_reached;
} AppState;

void run_app(void); 
void show_weight_input_screen(AppState *state); // shows weight input / beginning screen
void show_main_tracking_screen(AppState *state); // main screen of app-tracks water intake
float calculate_water_goal(float weight_lb); // calculates amt of water based on inputted weight
void draw_progress_bar(float progress); // updates progress bar with inputs
void clear_screen(void); //used in showing new screen
void print_header(void); // stays on top of the app for majority
void print_success(const char* message); // formats success messages 
void print_error(const char* message); // formats error messages
void print_info(const char* message); // formats information
bool check_validity(const char* input); // check validity of user input
#endif
