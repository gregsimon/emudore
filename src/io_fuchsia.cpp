/*
 * emudore, Commodore 64 emulator
 * Copyright (c) 2016, Mario Ballano <mballano@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdexcept>
#include "io.h"
#include "vic.h"


// clas ctor and dtor //////////////////////////////////////////////////////////

IO::IO()
{
  // TODO framebuffer init

  cols_ = Vic::kVisibleScreenWidth;
  rows_ = Vic::kVisibleScreenHeight;

  frame_  = new uint8_t[cols_ * rows_]();
  init_color_palette();
  init_keyboard();
  next_key_event_at_ = 0;
  prev_frame_was_at_ = std::chrono::high_resolution_clock::now();
}

IO::~IO()
{
  delete [] frame_;
  
  // TODO : framebuffer destroy
}

// init io devices  ////////////////////////////////////////////////////////////

/**
 * @brief init keyboard state and keymap
 */
void IO::init_keyboard()
{
  // TODO : create table of mojo keycodes to commodore keyboard
}

/** 
 * @brief init c64 color palette 
 */

uint8_t PackRGBtoRGB332(uint8_t r, uint8_t g, uint8_t b) {
  // r0r1r2 g0g1g2 b0b1
  return (r &  0xe0) | ( (g & 0xe0) >> 3) | ((b & 0xc0 ) >> 6);
}
void IO::init_color_palette()
{
  color_palette[0]   = PackRGBtoRGB332( 0x00, 0x00, 0x00);
  color_palette[1]   = PackRGBtoRGB332( 0xff, 0xff, 0xff);
  color_palette[2]   = PackRGBtoRGB332( 0xab, 0x31, 0x26);
  color_palette[3]   = PackRGBtoRGB332( 0x66, 0xda, 0xff);
  color_palette[4]   = PackRGBtoRGB332( 0xbb, 0x3f, 0xb8);
  color_palette[5]   = PackRGBtoRGB332( 0x55, 0xce, 0x58);
  color_palette[6]   = PackRGBtoRGB332( 0x1d, 0x0e, 0x97);
  color_palette[7]   = PackRGBtoRGB332( 0xea, 0xf5, 0x7c);
  color_palette[8]   = PackRGBtoRGB332( 0xb9, 0x74, 0x18);
  color_palette[9]   = PackRGBtoRGB332( 0x78, 0x53, 0x00);
  color_palette[10]  = PackRGBtoRGB332( 0xdd, 0x93, 0x87);
  color_palette[11]  = PackRGBtoRGB332( 0x5b, 0x5b, 0x5b);
  color_palette[12]  = PackRGBtoRGB332( 0x8b, 0x8b, 0x8b);
  color_palette[13]  = PackRGBtoRGB332( 0xb0, 0xf4, 0xac);
  color_palette[14]  = PackRGBtoRGB332( 0xaa, 0x9d, 0xef);
  color_palette[15]  = PackRGBtoRGB332( 0xb8, 0xb8, 0xb8);

/*
  color_palette[0]   = SDL_MapRGB(format_, 0x00, 0x00, 0x00);
  color_palette[1]   = SDL_MapRGB(format_, 0xff, 0xff, 0xff);
  color_palette[2]   = SDL_MapRGB(format_, 0xab, 0x31, 0x26);
  color_palette[3]   = SDL_MapRGB(format_, 0x66, 0xda, 0xff);
  color_palette[4]   = SDL_MapRGB(format_, 0xbb, 0x3f, 0xb8);
  color_palette[5]   = SDL_MapRGB(format_, 0x55, 0xce, 0x58);
  color_palette[6]   = SDL_MapRGB(format_, 0x1d, 0x0e, 0x97);
  color_palette[7]   = SDL_MapRGB(format_, 0xea, 0xf5, 0x7c);
  color_palette[8]   = SDL_MapRGB(format_, 0xb9, 0x74, 0x18);
  color_palette[9]   = SDL_MapRGB(format_, 0x78, 0x53, 0x00);
  color_palette[10]  = SDL_MapRGB(format_, 0xdd, 0x93, 0x87);
  color_palette[11]  = SDL_MapRGB(format_, 0x5b, 0x5b, 0x5b);
  color_palette[12]  = SDL_MapRGB(format_, 0x8b, 0x8b, 0x8b);
  color_palette[13]  = SDL_MapRGB(format_, 0xb0, 0xf4, 0xac);
  color_palette[14]  = SDL_MapRGB(format_, 0xaa, 0x9d, 0xef);
  color_palette[15]  = SDL_MapRGB(format_, 0xb8, 0xb8, 0xb8);
  */
}

// emulation /////////////////////////////////////////////////////////////////// 

bool IO::emulate()
{
  bool retval = true;

  /*
  SDL_Event event;
  // don't be greedy, just process one event per emulation cycle 
  if(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_KEYDOWN:
      handle_keydown(event.key.keysym.scancode);
      break;
    case SDL_KEYUP:
      handle_keyup(event.key.keysym.scancode);
      break;
    case SDL_QUIT:
      retval = false;
      break;
    }
  }
  // process fake keystrokes if any 
  if(!key_event_queue_.empty() && 
     cpu_->cycles() > next_key_event_at_)
  {
    std::pair<kKeyEvent,SDL_Keycode> &ev = key_event_queue_.front();
    key_event_queue_.pop();
    switch(ev.first)
    {
    case kPress:
      handle_keydown(ev.second);
      break;
    case kRelease:
      handle_keyup(ev.second);
      break;
    }
    next_key_event_at_ = cpu_->cycles() + kWait;
  }
  */
  return retval;
}

// keyboard handling /////////////////////////////////////////////////////////// 

/*
void IO::handle_keydown(SDL_Keycode k)
{
  try
  {
    uint8_t mask = ~(1 << keymap_.at(k).second);
    keyboard_matrix_[keymap_.at(k).first] &= mask;
  }
  catch(const std::out_of_range){}

void IO::handle_keyup(SDL_Keycode k)
{
  try
  {
    uint8_t mask = (1 << keymap_.at(k).second);
    keyboard_matrix_[keymap_.at(k).first] |= mask;
  }
  catch(const std::out_of_range){}  
}
*/

/**
 * @brief fake press a key, monkeys love it
 *
 * Characters are added to a queue and processed within 
 * the emulation loop.
 */
void IO::type_character(char c)
{
  /*
  try
  {
    for(SDL_Keycode &k: charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kPress,k));
    for(SDL_Keycode &k: charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kRelease,k));
  }
  catch(const std::out_of_range){}   
  */
}

// screen handling /////////////////////////////////////////////////////////////

void IO::screen_draw_rect(int x, int y, int n, int color)
{
  for(int i=0; i < n ; i++)
  {
    screen_update_pixel(x+i,y,color);
  }
}
 
void IO::screen_draw_border(int y, int color)
{
  screen_draw_rect(0,y,cols_,color);
}
 
/**
 * @brief refresh screen 
 *
 * Upload the texture to the GPU 
 */
void IO::screen_refresh()
{
  printf("IO::screen_refresh()\n");
  // TODO : copy pixmap to mojo framebuffer

  //SDL_UpdateTexture(texture_, NULL, frame_, cols_ * sizeof(uint32_t));
  //SDL_RenderClear(renderer_);
  //SDL_RenderCopy(renderer_,texture_, NULL, NULL);
  //SDL_RenderPresent(renderer_);
  vsync();
}

/**
 * @brief vsync
 *
 * vsync() is called at the end of every frame, if we are ahead 
 * of time compared to a real C64 (very likely) we sleep for a bit, 
 * this way we avoid running at full speed allowing the host CPU to 
 * take a little nap before getting back to work.
 *
 * This should also help with performance runing on slow computers, 
 * uploading data to the GPU is a relatively slow operation, doing 
 * more fps obviously has a performance impact.
 *
 * Also, and more importantly, by doing this we emulate the actual 
 * speed of the C64 so visual effects do not look accelerated and 
 * games become playable :)
 */
void IO::vsync()
{
  using namespace std::chrono;
  auto t = high_resolution_clock::now() - prev_frame_was_at_;
  duration<double> rr(Vic::kRefreshRate);
  /**
   * Microsoft's chrono is buggy and does not properly handle 
   * doubles, we need to recast to milliseconds.
   */
  auto ttw = duration_cast<milliseconds>(rr - t);
  std::this_thread::sleep_for(ttw);
  prev_frame_was_at_ = std::chrono::high_resolution_clock::now();
}
