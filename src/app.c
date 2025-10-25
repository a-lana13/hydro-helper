#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "app.h"

#define PROGRESS_BAR_WIDTH 50

void clear_screen(void) {
    printf("\033[2J\033[H"); // ANSI- clear screen
}

void print_header(void) { //header stays on top of screen for most of app
    printf(COLOR_BOLD COLOR_CYAN);
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║                 "COLOR_BLUE"💧 HYDRO HELPER"COLOR_CYAN"                  ║\n");
    printf("║            "COLOR_WHITE"Personal Hydration Tracker"COLOR_CYAN"            ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
}

void print_success(const char* message) {
    printf(COLOR_BOLD COLOR_GREEN"✅ %s"COLOR_RESET"\n", message);
}

void print_error(const char* message) {
    printf(COLOR_BOLD COLOR_RED"❌ %s"COLOR_RESET"\n", message);
}

void print_info(const char* message) {
    printf(COLOR_CYAN"💡 %s"COLOR_RESET"\n", message);
}

float calculate_water_goal(float weight_lb) {
    return weight_lb*0.5f; // .5 ounce per pound of body weight
}

void draw_progress_bar(float progress) {
    int filled_width = (int)(PROGRESS_BAR_WIDTH*progress);
    if(filled_width>PROGRESS_BAR_WIDTH) filled_width=PROGRESS_BAR_WIDTH;
    
    // color of bar is based on progress
    const char* color;
    if(progress<0.25){
	   color=COLOR_RED;
    }else if(progress<0.5){ 
	   color=COLOR_YELLOW;
    }else if(progress<1.0){
	   color=COLOR_BLUE;
    }else{ 
	   color=COLOR_GREEN;
    }
    
    printf(COLOR_BOLD "[");
    for(int i=0; i<PROGRESS_BAR_WIDTH; i++) { // filling/coloring progress bar
        if(i<filled_width){
            printf("%s█"COLOR_RESET COLOR_BOLD, color);
        }else{
            printf(" ");
        }
    }
    printf("]"COLOR_RESET);
    printf(" %.1f%%\n", progress*100);
}

const char* get_encouragement_message(float progress) {
    if(progress<0.25f) return COLOR_BLUE"💧 Better get drinkin'..."COLOR_RESET;
    if(progress<0.5f) return COLOR_CYAN"🚀 Alright! Making some progress..."COLOR_RESET;
    if(progress<0.75f) return COLOR_MAGENTA"⭐️ Over halfway there!"COLOR_RESET;
    if(progress<1.0f) return COLOR_YELLOW"🎯 Just a little more!"COLOR_RESET;
    return COLOR_BOLD COLOR_GREEN"🎉 Congrats! You're hydrated!"COLOR_RESET;
}

bool check_validity(const char* input) { // validates user input
    if(strlen(input)== 0) return false; // check for empty input
    
    int decimal_points = 0; // decimal counter
    int has_digit = false;

    for(int i = 0; input[i]!= '\0'; i++) {
        if(isdigit(input[i])){ // check for digits
            has_digit = true;
        }else if(input[i] == '.'){ // decimal checker
            decimal_points++;
            if(decimal_points > 1) return false; // if > one decimal point, invalid
        }else{ // other char invalid
            return false;
        }
    }
 
    return has_digit;
}

void show_weight_input_screen(AppState *state) { //beginning screen
    char input[20]; //buffer
    
    while(1){
        clear_screen();
        print_header();
        printf("\n");
        
        printf(COLOR_BOLD COLOR_WHITE"Enter your weight in pounds: "COLOR_RESET);
        
        if(fgets(input, sizeof(input), stdin)!=NULL){ //checks for valid input (not ctrl+c)
            input[strcspn(input, "\n")] = 0; // removes new line char
            
            if(check_validity(input)){ //checking validity of input
            	state->weight = atof(input); // input str --> float

                if(state->weight >0 && state->weight <1000) { // bounds check for weight
                    // initializing var
		    state->daily_goal = calculate_water_goal(state->weight); 
                    state->consumed = 0.0f;
                    state->goal_reached = 0;
                    print_success("Goal calculated successfully!");
                    printf(COLOR_WHITE "Your daily water goal: " COLOR_BOLD COLOR_CYAN "%.2foz" COLOR_RESET "\n\n", state->daily_goal);
                    printf("Press Enter to continue..." COLOR_RESET);
                    getchar();
                    break;
                }
            }
	    
	    //invalid case
            print_error("Invalid input! Please enter a valid weight (e.g., 127.5)");
            printf("Press Enter to try again...");
            getchar();
        }
    }
}

void show_main_tracking_screen(AppState *state) { //main screen of the app
    char input[20]; //buffer
    
    while(1){
        clear_screen();
        print_header();
        printf("\n");
        
        // display current stats
        printf(COLOR_WHITE"Personal Goal: "COLOR_CYAN"%.2foz"COLOR_WHITE" (based on "COLOR_CYAN"%.1f lb"COLOR_WHITE")\n\n", state->daily_goal, state->weight);
        
        printf(COLOR_BOLD"Current Progress:\n"COLOR_RESET);
        
        float progress = state->daily_goal > 0 ? state->consumed / state->daily_goal : 0;
        draw_progress_bar(progress);
        
        printf("\n"COLOR_WHITE"%.2foz/ %.2foz consumed\n\n"COLOR_RESET, state->consumed, state->daily_goal);
        
        // encouragement message
        printf("%s\n\n", get_encouragement_message(progress));
        
        // goal reached
        if(progress>= 1.0f && !state->goal_reached){
            state->goal_reached = 1;
            printf(COLOR_BOLD BG_GREEN COLOR_WHITE);
            printf("╔══════════════════════════════════════════════════╗\n");
            printf("║       🎉 GOAL ACHIEVED! CONGRATULATIONS! 🎉      ║\n");
            printf("╚══════════════════════════════════════════════════╝\n");
            printf(COLOR_RESET "\n");
        }else if(state->goal_reached){ // when user continues inputting after goal reached
            printf(COLOR_BOLD COLOR_GREEN"🎉 Goal already achieved today! Great job! 🎉\n\n"COLOR_RESET);
        }
        
        // menu options
        printf(COLOR_BOLD COLOR_WHITE"Options:\n"COLOR_RESET);
        printf(COLOR_CYAN"1"COLOR_RESET"-"COLOR_CYAN"3"COLOR_RESET": Quick add water ("COLOR_GREEN"4oz"COLOR_RESET", "COLOR_BLUE"8oz"COLOR_RESET", "COLOR_MAGENTA"16oz"COLOR_RESET")\n");
        printf(COLOR_YELLOW"C"COLOR_RESET": Custom amount\n");
        printf(COLOR_RED"R"COLOR_RESET": Reset daily consumption\n");
        printf(COLOR_CYAN"N"COLOR_RESET": New user (change weight)\n");
        printf(COLOR_WHITE"Q"COLOR_RESET": Quit\n\n");
        
        printf(COLOR_BOLD"Choose an option: "COLOR_RESET);
        
        if(fgets(input, sizeof(input), stdin) != NULL){
            input[strcspn(input, "\n")] = 0;
            
            switch(toupper(input[0])){
                case '1': // add 4oz option
                    state->consumed += 4.0f;
                    print_success("Added 4oz of water!");
                    break;
                case '2': //add 8oz option
                    state->consumed += 8.0f;
                    print_success("Added 8oz of water!");
                    break;
                case '3': //add 16oz option (based off standard plastic water bottle)
                    state->consumed += 16.0f;
                    print_success("Added 16oz of water!");
                    break;
                case 'C': { // custom amt option
                    printf(COLOR_WHITE"Enter custom amount in ounces: "COLOR_RESET);
                    if(fgets(input, sizeof(input), stdin) != NULL){
                    	input[strcspn(input, "\n")] = 0; //rm /n

			if(check_validity(input)){
			    float custom_amount = atof(input); // input str --> float
                            state->consumed += custom_amount; //add custom amt
                            printf(COLOR_GREEN "✅ Added %.2f oz of water!\n" COLOR_RESET, custom_amount);
                        }else{
                            print_error("Invalid amount!");
                        }
                    }
                    printf("Press Enter to continue...");
                    getchar();
                    break;
                }
                case 'R': //reset option
                    state->consumed = 0.0f;
                    state->goal_reached = 0;
                    print_info("Daily consumption reset to 0!");
                    printf("Press Enter to continue...");
                    getchar();
                    break;
                case 'N': // enter different weight option (new user)
                    show_weight_input_screen(state);
                    break;
                case 'Q': // quit option
                    return;
                default: //catch case
                    print_error("Invalid option!");
                    printf("Press Enter to continue...");
                    getchar();
                    break;
            }
        }
    }
}

void run_app(void) {
    AppState state = {0};
    
    show_weight_input_screen(&state);
    show_main_tracking_screen(&state);
    
    clear_screen();
    printf(COLOR_BOLD COLOR_CYAN);
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║         Thank you for using Hydro Helper!        ║\n");
    printf("║                   Stay moist! 💧                 ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
}
