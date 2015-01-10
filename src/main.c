#include <pebble.h>
#include "main.h"

static Window *titleWindow;
static Window *gameWindow;
static TextLayer *titleText;
static TextLayer *startText;
static TextLayer *testText;
static TextLayer *s_output_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
  //text_layer_set_text(s_output_layer, "Select pressed!");
  window_stack_push(gameWindow, true);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
  text_layer_set_text(s_output_layer, "Up pressed!");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context){
  text_layer_set_text(s_output_layer, "Down pressed!");
}


static void click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
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
  text_layer_set_text(s_output_layer, "fuck you");
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
  text_layer_set_text(testText, "gAmE oN kIdDoS!");
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
}


int main(void){
  init();
  app_event_loop();
  deinit();
}