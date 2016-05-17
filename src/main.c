#include <pebble.h>
// created 	29 March, 2016
#define UPDATE_MS 30
static Window *window;
static Layer  *s_layer;
static int s_secondAngle;
int32_t ang = 0;

int32_t abs32(int32_t a) {return (a^(a>>31)) - (a>>31);}       // returns absolute value of A (only works on 32bit signed)

// GPoint getPointOnRect(int angle, GRect r) {
//   int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);  // Calculate once and store, to make quicker and cleaner
//   int32_t dy = sin>0 ? (r.size.h/2) : (0-(r.size.h)/2);                  // Distance to top or bottom edge (from center)
//   int32_t dx = cos>0 ? (r.size.w/2) : (0-(r.size.w)/2);                  // Distance to left or right edge (from center)
//   if(abs32(dx*sin) < abs32(dy*cos)) {                        // if (distance to vertical line) < (distance to horizontal line)
//     dy = (dx * sin) / cos;                                   // calculate distance to vertical line
//   } else {                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
//     dx = (dy * cos) / sin;                                   // move to top or bottom line
//   }
//   return GPoint(dx+r.origin.x+(r.size.w/2), dy+r.origin.y+(r.size.h/2));                                     // Return point on rectangle
// }

// Undefined if center is outside rect
// GPoint getPointOnRect_center(int angle, GRect r, GEdgeInsets inset, GPoint center) {  // Inset of 0 to get the point ON the rect
//   int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);     // Calculate once and store, to make quicker and cleaner
//   int32_t dy = (r.origin.y - center.y) + (sin>0 ? (r.size.h - 1) : 0); // Distance to top or bottom edge (from center)
//   int32_t dx = (r.origin.x - center.x) + (cos>0 ? (r.size.w - 1) : 0); // Distance to left or right edge (from center)
//   if(abs32(dx*sin) < abs32(dy*cos)) {                           // if (distance to vertical line) < (distance to horizontal line)
//     dy = (dx * sin) / cos;                                      // calculate y position on left or right edge
//     dx -= cos>0 ? inset.right : -inset.left;                    // Bring point horizontally inward (unless inset is negative)
//   } else {                                                      // else: (distance to top or bottom edge) < (distance to left or right edge)
//     dx = (dy * cos) / sin;                                      // calculate x position on top or bottom line
//     dy -= sin>0 ? inset.bottom : -inset.top;                    // Bring point vertically inward (unless inset is negative)
//   }
//   return GPoint(dx+center.x, dy+center.y);  // Return point on rectangle
// }

// Undefined if center is on or outside rect
GPoint getPointOnRect_offcenter(int angle, GRect r, GPoint center) {  // Inset of 0 to get the point ON the rect
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);           // Calculate once and store, to make quicker and cleaner
  int32_t dy = r.origin.y - center.y + (sin>0 ? (r.size.h - 1) : 0);  // Distance to top or bottom edge (from center)
  int32_t dx = r.origin.x - center.x + (cos>0 ? (r.size.w - 1) : 0);  // Distance to left or right edge (from center)
  if(abs32(dx*sin) < abs32(dy*cos)) {                                 // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                            // calculate y position on left or right edge
  } else {                                                            // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                            // calculate x position on top or bottom line
  }
  return GPoint(dx+center.x, dy+center.y);                            // Return point on rectangle
}

GPoint getPointOnRect_inset(int angle, GRect r, GEdgeInsets inset) {      // Inset of 0 to get the point ON the rect
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);               // Calculate once and store, to make quicker and cleaner
  int32_t dy = (sin>0 ? r.size.h : (0 - r.size.h)) / 2;                   // Distance to top or bottom edge (from center)
  int32_t dx = (cos>0 ? r.size.w : (0 - r.size.w)) / 2;                   // Distance to left or right edge (from center)
  if(abs32(dx*sin) < abs32(dy*cos)) {                                     // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                                // calculate y position on left or right edge
    dx -= cos>0 ? inset.right : -inset.left;                              // Bring point horizontally inward (unless inset is negative)
  } else {                                                                // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                                // calculate x position on top or bottom line
    dy -= sin>0 ? inset.bottom : -inset.top;                              // Bring point vertically inward (unless inset is negative)
  }
  return GPoint(dx+r.origin.x+(r.size.w/2), dy+r.origin.y+(r.size.h/2));  // Return point on rectangle
}

// GPoint getPointOnRect(int angle, GRect r, int inset) {       // Inset of 0 to get the point ON the rect
//   int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);  // Calculate once and store, to make quicker and cleaner
//   int32_t dy = sin>0 ? (r.size.h/2) : (0-r.size.h)/2;        // Distance to top or bottom edge (from center)
//   int32_t dx = cos>0 ? (r.size.w/2) : (0-r.size.w)/2;        // Distance to left or right edge (from center)
//   if(abs32(dx*sin) < abs32(dy*cos)) {                        // if (distance to vertical line) < (distance to horizontal line)
//     dy = (dx * sin) / cos;                                   // calculate y position on left or right edge
//     dx -= cos>0 ? inset : -inset;                            // Bring point horizontally inward [inset] pixels
//   } else {                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
//     dx = (dy * cos) / sin;                                   // calculate x position on top or bottom line
//     dy -= sin>0 ? inset : -inset;                            // Bring point vertically inward [inset] pixels
//   }
//   return GPoint(dx+r.origin.x+(r.size.w/2), dy+r.origin.y+(r.size.h/2));  // Return point on rectangle
// }

GPoint getPointOnRect(int angle, GRect r) {
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);  // Calculate once and store, to make quicker and cleaner
  int32_t dy = sin>0 ? (r.size.h/2) : (0-r.size.h)/2;        // Distance to top or bottom edge (from center)
  int32_t dx = cos>0 ? (r.size.w/2) : (0-r.size.w)/2;        // Distance to left or right edge (from center)
  if(abs32(dx*sin) < abs32(dy*cos)) {                        // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                   // calculate distance to vertical line
  } else {                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                   // move to top or bottom line
  }
  return GPoint(dx+r.origin.x+(r.size.w/2), dy+r.origin.y+(r.size.h/2));                                     // Return point on rectangle
}

GPoint ORIGINALgetPointOnRect(int angle, int w, int h) {
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);  // Calculate once and store, to make quicker and cleaner
  int32_t dy = sin>0 ? ((h-1)/2) : (0-h)/2;                  // Distance to top or bottom edge (from center)
  int32_t dx = cos>0 ? ((w-1)/2) : (0-w)/2;                  // Distance to left or right edge (from center)
  if(abs32(dx*sin) < abs32(dy*cos)) {                        // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                   // calculate distance to vertical line
  } else {                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                   // move to top or bottom line
  }
  return GPoint(dx, dy);                                     // Return point on rectangle
}

static void updateProc(Layer* thisLayer, GContext* ctx) {
  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  //static GPoint _centre = {.x = 144/2, .y = 168/2};
  //static GSize _boundA = {.w = (90*144)/100, .h = (90*168)/100};
  //static GSize _boundB = {.w = (70*144)/100, .h = (70*168)/100};
  
  GRect f = layer_get_frame(thisLayer);
  GPoint center;
  //center = (GPoint){.x = f.origin.x + f.size.w/2, .y = f.origin.y + f.size.h/2};
  center = (GPoint){.x = f.origin.x + f.size.w, .y = f.origin.y + f.size.h};
  
  //GRect rectA; rectA = GRect(20, 20, f.size.w-40, f.size.h-40);
  GRect rectA; rectA = GRect(20, 20, (rand()%50)+70, (rand()%50)+70);
  //GRect rectB; rectB = GRect(40, 40, f.size.w-80, f.size.h-80);
  //GRect rectA = GRect(20, 20, 81, 31);
  //GRect rectB = GRect(10, 10, 101, 51);
  center = (GPoint){.x = rectA.origin.x + rectA.size.w-20, .y = rectA.origin.y + rectA.size.h-20};
  center = (GPoint){.x = rectA.origin.x + rectA.size.w/2, .y = rectA.origin.y + rectA.size.h/2};  
  
  // Draw Rectangles
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, rectA);
  //graphics_draw_rect(ctx, rectB);
  
  // Draw Secondhand
  graphics_context_set_stroke_color(ctx, GColorRed);
  GPoint secondHand;
  secondHand.y = (sin_lookup(s_secondAngle) * 200 / TRIG_MAX_RATIO) + center.y;
  secondHand.x = (cos_lookup(s_secondAngle) * 200 / TRIG_MAX_RATIO) + center.x;
  graphics_draw_line(ctx, secondHand, center);

  // Draw Circle
  graphics_context_set_fill_color(ctx, GColorYellow);
//   GPoint result = getPointOnRect(ang, rectA, GEdgeInsets1(0));
//   GPoint result = getPointOnRect_center(ang, rectA, GEdgeInsets1(0), center);
  GPoint result = getPointOnRect_offcenter(ang, rectA, center);
  graphics_fill_circle(ctx, result, 3);
  graphics_context_set_fill_color(ctx, GColorBlue);
  graphics_draw_pixel(ctx, result);

  // Draw Green Lines
  graphics_context_set_stroke_color(ctx, GColorGreen);
  for (int a = 0; a < TRIG_MAX_ANGLE; a += (TRIG_MAX_ANGLE / 60)) {
//     GPoint pA = getPointOnRect(a, rectA, GEdgeInsets1(0));
//     GPoint pA = getPointOnRect_center(a, rectA, GEdgeInsets1(0), center);
//     GPoint pA = getPointOnRect_offcenter(a, rectA, center);
    //GPoint pA = getPointOnRect(a, rectA);
    //GPoint pB = getPointOnRect(a, f);
    GPoint pA = getPointOnRect_offcenter(a, rectA, center);
    GPoint pB = getPointOnRect_offcenter(a, f, center);
    graphics_draw_line(ctx, pA, pB);
    //graphics_draw_line(ctx, pA, center);
  }

  
  //graphics_fill_circle(_ctx, _pA, 2);
  
  //static GPoint centre; centre = (GPoint){.x = (f.size.w-1)/2, .y = (f.size.h-1)/2};
  //static GSize _boundA; _boundA = (GSize){.w = (90*f.size.w)/100, .h = (90*f.size.h)/100};
  //static GSize _boundB; _boundB = (GSize){.w = (70*f.size.w)/100, .h = (70*f.size.h)/100};

  
  
  
//   graphics_context_set_stroke_color(_ctx, GColorWhite);
//   GRect _rA = GRect(_centre.x - _boundA.w/2, _centre.y - _boundA.h/2, _boundA.w, _boundA.h);
//   GRect _rB = GRect(_centre.x - _boundB.w/2, _centre.y - _boundB.h/2, _boundB.w, _boundB.h);
//   graphics_draw_rect(_ctx, _rA);
//   graphics_draw_rect(_ctx, _rB);


  
  graphics_context_set_stroke_color(ctx, GColorBlue);
  graphics_draw_pixel(ctx, center);
  // For each tick mark
//   for (int _a = 0; _a < TRIG_MAX_ANGLE; _a += (TRIG_MAX_ANGLE / 8)) {
//     GPoint _pA = getPointOnRect(_a, _boundA.w, _boundA.h);
//     GPoint _pB = getPointOnRect(_a, _boundB.w, _boundB.h);
//     _pA.x += _centre.x;
//     _pB.x += _centre.x;
//     _pA.y += _centre.y;
//     _pB.y += _centre.y;
//     //graphics_fill_circle(_ctx, _pA, 2);
//     graphics_draw_line(_ctx, _pA, _pB);
//   }
//   graphics_context_set_stroke_color(_ctx, GColorRed);
//   GPoint _pA = getPointOnRect(ang, _boundA.w, _boundA.h);
//   GPoint _pB = getPointOnRect(ang, _boundB.w, _boundB.h);
//     _pA.x += _centre.x;
//     _pB.x += _centre.x;
//     _pA.y += _centre.y;
//     _pB.y += _centre.y;
//     graphics_draw_line(_ctx, _pA, _pB);
}

void tickHandler(struct tm* tickTime, TimeUnits unitsChanged) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Second: %d", (int)tickTime->tm_sec);
  s_secondAngle = TRIG_MAX_ANGLE * (tickTime->tm_sec-15) / 60;
  ang = s_secondAngle;
  layer_mark_dirty(s_layer);
}

static void loop(void *data) {
 ang+=TRIG_MAX_ANGLE/(60*30);
 layer_mark_dirty(s_layer);  // Schedule redraw of screen
 app_timer_register(UPDATE_MS, loop, NULL); // Finished. Wait UPDATE_MS then loop
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_frame(window_layer);
  s_layer = layer_create(window_frame);
  //s_layer = layer_create(GRect(20,20,80,81));
  layer_set_update_proc(s_layer, updateProc);
  layer_add_child(window_layer, s_layer);
  tick_timer_service_subscribe(SECOND_UNIT, tickHandler);
  //app_timer_register(UPDATE_MS, loop, NULL);
}

void init() {
  window = window_create();                                                    // Create window
  window_set_window_handlers(window, (WindowHandlers) {.load = window_load});  // Set window load handler
  window_set_background_color(window, GColorBlack);//IF_BW_COLOR(GColorWhite, GColorVividCerulean));
  window_stack_push(window, false);                                            // Push window to stack
}

int main(void) {
  init();
  app_event_loop();
  layer_destroy(s_layer);
  window_destroy(window);                                                      // Destroy window
}