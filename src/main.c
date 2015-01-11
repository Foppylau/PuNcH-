#include <pebble.h>
#include "main.h"

static GBitmap *titleScreen;
static GBitmap *gameScreen;
static BitmapLayer *zombieLayer;
static BitmapLayer *titleLayer;
static Window *titleWindow;
static Window *gameWindow;
static Window *endWindow;
static BitmapLayer *endLayer;
static GBitmap *endScreen;
static TextLayer *startText;
static TextLayer *testText;
static TextLayer *dmgText;
static TextLayer *HPText;
static AppTimer *punch_timer;
static AppTimer *fight_timer;
static AppTimer *stats_timer;
static AppTimer *delay_timer;
static int MAX_PUNCH = 0;
static int USER_HP;
static int ENEMY_HP;
static bool bossFlag = false;
static bool winStatus = false;

static int absoluteValue(int input){
  if(input < 0){
    input *= -1;
  }
  return input;
}

static int randomNum(int low, int high){
  int value = rand();
  value = value % (high-low);
  return (value + low);
}

static void delay_timer_callback(void *data){
  text_layer_set_text(dmgText, "");
  text_layer_set_text(HPText, "");
  window_stack_remove(gameWindow, true);
  window_stack_push(endWindow, true);
}

static void fightZamby(void *data){
  MAX_PUNCH = 0;
  //To-Do: Upload image of zombie
  text_layer_set_text(testText, "GET READY...");
  punch_timer = app_timer_register(1000, punch_timer_callback, NULL);
}

static void data_handler(AccelData *data, uint32_t num_samples){
  
  int magnitude = absoluteValue(data[0].x);
  magnitude += absoluteValue(data[0].y);
  magnitude += absoluteValue(data[0].z);
  if (magnitude > THRESHOLD && magnitude > MAX_PUNCH){
      MAX_PUNCH = (magnitude - 1100);
  }
}

static void fight_timer_callback(void *data){
  accel_data_service_unsubscribe();
  int enemyDmg = randomNum(500, 2500);
  ENEMY_HP -= MAX_PUNCH;
  USER_HP -= enemyDmg;
  
  if(MAX_PUNCH >= 3000){
    text_layer_set_text(testText, "MEGA PUNCH!");
  }
  else if(MAX_PUNCH >= 2000){
    text_layer_set_text(testText, "Sweet Hit");
  }
  else if(MAX_PUNCH >= 1500){
    text_layer_set_text(testText, "Not Bad");
  }
  else if(MAX_PUNCH >= 1000){
    text_layer_set_text(testText, "Try Harder");
  }
  else{
    text_layer_set_text(testText, "C'mon Man...");
  }
  
  static char zombieHP[20];
  static char userHP[20];
  snprintf(zombieHP, sizeof(zombieHP), "Zombie HP: %d", ENEMY_HP);
  snprintf(userHP, sizeof(userHP), "Player HP: %d", USER_HP);
  text_layer_set_text(dmgText, zombieHP);
  text_layer_set_text(HPText, userHP);
  
  if(ENEMY_HP <= 0 || USER_HP <= 0){
    if(ENEMY_HP <= 0){
      winStatus = true;
    }
    else if(USER_HP <= 0){
      winStatus = false;
    }
    delay_timer = app_timer_register(2000, delay_timer_callback, NULL);
  }
  else{
      stats_timer = app_timer_register(2000, fightZamby, NULL);
  }
}

static void punch_timer_callback(void *data){
 text_layer_set_text(testText, "PUNCH!");
  
  int num_samples = 3;
  accel_data_service_subscribe(num_samples, data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  fight_timer = app_timer_register(1500, fight_timer_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
  Window *checkGame = window_stack_get_top_window();
  if(checkGame != gameWindow){
    window_stack_push(gameWindow, true);
    
    int n = randomNum(0,6);
    USER_HP = 7000;
    if(n == 5){
      bossFlag = true;
      ENEMY_HP = 15000;
    }
    else{
      bossFlag = false;
      ENEMY_HP = 8000;
    }
    fightZamby(NULL);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
  
}

static void click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void title_window_load(Window *window){
  titleScreen = gbitmap_create_with_resource(RESOURCE_ID_TITLE);
  titleLayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(titleLayer, titleScreen);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(titleLayer));
  
  //Display Select to Play text
  startText = text_layer_create(GRect(0, 100, 144, 35));
  text_layer_set_background_color(startText, GColorClear);
  text_layer_set_text_color(startText, GColorBlack);
  text_layer_set_text(startText, "Press Select to Play.");
  text_layer_set_font(startText, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(startText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(startText));
}

static void title_window_unload(Window *window){
  text_layer_destroy(startText);
  gbitmap_destroy(titleScreen);
  bitmap_layer_destroy(titleLayer);
}

static void game_window_load(Window *window){
  gameScreen = gbitmap_create_with_resource(RESOURCE_ID_ZOMBIE);
  zombieLayer = bitmap_layer_create(GRect(45, 3, 59, 60));
  bitmap_layer_set_bitmap(zombieLayer, gameScreen);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(zombieLayer));
  
  testText = text_layer_create(GRect(0, 50, 144, 40));
  text_layer_set_background_color(testText, GColorClear);
  text_layer_set_text_color(testText, GColorBlack);
  
  text_layer_set_text(testText, "");
  text_layer_set_font(testText, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(testText, GTextAlignmentCenter);
  text_layer_set_overflow_mode(testText, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(testText));
  
  dmgText = text_layer_create(GRect(0, 100, 144, 34));
  text_layer_set_background_color(dmgText, GColorClear);
  text_layer_set_text_color(dmgText, GColorBlack);
  
  text_layer_set_text(dmgText, "");
  text_layer_set_font(dmgText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(dmgText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dmgText));
  
  HPText = text_layer_create(GRect(0, 120, 144, 34));
  text_layer_set_background_color(HPText, GColorClear);
  text_layer_set_text_color(HPText, GColorBlack);
  
  text_layer_set_text(HPText, "");
  text_layer_set_font(HPText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(HPText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(HPText));
  
  
}

static void game_window_unload(Window *window){
  text_layer_destroy(testText);
  gbitmap_destroy(gameScreen);
  bitmap_layer_destroy(zombieLayer);
}

static void end_window_load(Window *window){
  endScreen = gbitmap_create_with_resource(RESOURCE_ID_LOSER);
  if(winStatus == true) endScreen = gbitmap_create_with_resource(RESOURCE_ID_WINNER);
  endLayer = bitmap_layer_create(GRect(0, -22, 144, 168));
  bitmap_layer_set_bitmap(endLayer, endScreen);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(endLayer));
}

static void end_window_unload(Window *window){
  gbitmap_destroy(endScreen);
  bitmap_layer_destroy(endLayer);
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
  endWindow = window_create();
  window_set_window_handlers(endWindow, (WindowHandlers){
    .load = end_window_load,
    .unload = end_window_unload
  });
  window_set_click_config_provider(gameWindow, click_config_provider);
}

static void deinit(){
  window_destroy(titleWindow);
  window_destroy(gameWindow);
  window_destroy(endWindow);
}


int main(void){
  init();
  app_event_loop();
  deinit();
}