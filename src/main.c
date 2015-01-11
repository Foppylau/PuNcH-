#include <pebble.h>
#include "main.h"
  
static Window *titleWindow;
static Window *gameWindow;
static TextLayer *titleText;
static TextLayer *startText;
static TextLayer *testText;
static TextLayer *s_output_layer;
static AppTimer *punch_timer;
static AppTimer *fight_timer;
static int currentCountdown;
static int MAX_PUNCH = 0;

static int absoluteValue(int input){
  if(input < 0){
    input *= -1;
  }
  return input;
}

static void data_handler(AccelData *data, uint32_t num_samples){
  static char s_buffer[128];
  //Compose string of data
  snprintf(s_buffer, sizeof(s_buffer),
          "%d,%d,%d",
          data[0].x, data[0].y, data[0].z
  );
  int magnitude = absoluteValue(data[0].x);
  magnitude += absoluteValue(data[0].y);
  magnitude += absoluteValue(data[0].z);
  if (magnitude > THRESHOLD){
      MAX_PUNCH += (magnitude - 1100);
      snprintf(s_buffer, sizeof(s_buffer), "MAX: %d", MAX_PUNCH);
      text_layer_set_text(testText, s_buffer);
  }
}

static void fight_timer_callback(void *data){
  accel_data_service_unsubscribe();
  text_layer_set_font(testText, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_overflow_mode(testText, GTextOverflowModeFill);
  if(MAX_PUNCH < HITPOINTS){
    text_layer_set_text(testText, "You've Been Eaten!");
  }
  else{
    text_layer_set_text(testText, "You've Slain the Zombie");
  }
  MAX_PUNCH = 0;
}

static void punch_timer_callback(void *data){
 text_layer_set_text(testText, "PUNCH!");
  
  int num_samples = 3;
  accel_data_service_subscribe(num_samples, data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  fight_timer = app_timer_register(5000, fight_timer_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
  Window *checkGame = window_stack_get_top_window();
  if(checkGame == titleWindow){
    window_stack_push(gameWindow, true);
    
    punch_timer = app_timer_register(1000, punch_timer_callback, NULL);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
  MAX_PUNCH = 0;
  text_layer_set_text(testText, "PUNCH!");
}

static void click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void title_window_load(Window *window){
  //Display title text
  titleText = text_layer_create(GRect(0, 50, 144, 40));
  text_layer_set_background_color(titleText, GColorBlack);
  text_layer_set_text_color(titleText, GColorWhite);
  text_layer_set_text(titleText, "PuNcH!");
  text_layer_set_font(titleText, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(titleText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(titleText));
  
  //Display Select to Play text
  startText = text_layer_create(GRect(0, 100, 144, 35));
  text_layer_set_background_color(startText, GColorClear);
  text_layer_set_text_color(startText, GColorBlack);
  text_layer_set_text(startText, "Press Select to Play.");
  text_layer_set_font(startText, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(startText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(startText));
  
  //Test codes for buttons
  s_output_layer = text_layer_create(GRect(0, 10, 144,20));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text(s_output_layer, "love you");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_output_layer));
}

static void title_window_unload(Window *window){
  text_layer_destroy(titleText);
  text_layer_destroy(startText);
}

static void game_window_load(Window *window){
  testText = text_layer_create(GRect(0, 50, 144, 40));
  text_layer_set_background_color(testText, GColorBlack);
  text_layer_set_text_color(testText, GColorWhite);
  
  text_layer_set_text(testText, "GET READY...");
  text_layer_set_font(testText, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(testText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(testText));
}

static void game_window_unload(Window *window){
  text_layer_destroy(testText);
}


static void init(){
  titleWindow = window_create();
  window_set_window_handlers(titleWindow, (WindowHandlers){
    .load = title_window_load,
    .unload = title_window_unload
  });
  window_stack_push(titleWindow, true);
  window_set_click_config_provider(titleWindow, click_config_provider);
  gameWindow = window_create();
  window_set_window_handlers(gameWindow, (WindowHandlers){
    .load = game_window_load,
    .unload = game_window_unload
  });
  window_set_click_config_provider(gameWindow, click_config_provider);
}

static void deinit(){
  window_destroy(titleWindow);
  window_destroy(gameWindow);
}


int main(void){
  init();
  app_event_loop();
  deinit();
}