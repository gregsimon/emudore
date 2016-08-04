/*
 * emudore, Commodore 64 emulator
 * Copyright (c) 2016, Mario Ballano <mballano@gmail.com>
 * Added changes by Greg Simon <gregsimon@gmail.com>
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
 
 #include <magenta/syscalls.h>

#include "mojo/public/c/system/main.h"
#include "mojo/public/cpp/application/application_impl_base.h"
#include "mojo/public/cpp/application/connect.h"
#include "mojo/public/cpp/application/run_application.h"
#include "mojo/public/cpp/application/service_provider_impl.h"
#include "mojo/public/cpp/bindings/interface_request.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "mojo/public/cpp/system/macros.h"
#include "mojo/public/cpp/utility/run_loop.h"
#include "mojo/services/framebuffer/interfaces/framebuffer.mojom.h"

#include <iostream>
#include <string>
#include <algorithm>

#include "c64.h"
#include "io.h"


namespace {

class EmudoreApp : public mojo::ApplicationImplBase {
public:
  EmudoreApp() {}
  ~EmudoreApp() override {}

  void OnInitialize() override {
    mojo::ConnectToService(shell(), "mojo:framebuffer",
                           mojo::GetProxy(&provider_));
    CreateFramebuffer();
    c64_ = new C64();
  }

  void CreateFramebuffer() { provider_->Create(CallCreateSurface(this)); }

  void CreateSurface(mojo::InterfaceHandle<mojo::Framebuffer> frame_buffer,
                     mojo::FramebufferInfoPtr info) {
    if (!frame_buffer) {
      fprintf(stderr, "Failed to create frame buffer\n");
      return;
    }
    frame_buffer_.Bind(std::move(frame_buffer));
    info_ = std::move(info);

    
    size_t row_bytes = info_->row_bytes;
    size_t size = row_bytes * info_->size->height;

    mx_status_t status =
        mx_process_vm_map(0, info_->vmo.get().value(), 0, size, &buffer_,
                          MX_VM_FLAG_PERM_READ | MX_VM_FLAG_PERM_WRITE);

    if (status < 0) {
      fprintf(stderr, "Cannot map frame buffer %d\n", status);
      mojo::RunLoop::current()->Quit();
      return;
    }

    /*
    SkColorType sk_color_type;
    switch (info_->format) {
      case mojo::FramebufferFormat::RGB_565:
        sk_color_type = kRGB_565_SkColorType;
        break;
      case mojo::FramebufferFormat::ARGB_8888:
        sk_color_type = kRGBA_8888_SkColorType;
        break;
      default:
        printf("Unknown color type %d\n", info_->format);
        sk_color_type = kRGB_565_SkColorType;
        break;
    }
    SkImageInfo image_info =
        SkImageInfo::Make(info_->size->width, info_->size->height,
                          sk_color_type, kPremul_SkAlphaType);

    surface_ = SkSurface::MakeRasterDirect(
        image_info, reinterpret_cast<void*>(buffer), row_bytes);
        */

    mojo::RunLoop::current()->PostDelayedTask([this]() { Draw(); }, 0);
  }

  void Draw() {
    printf("EmudoreApp::Draw() fmt=%d\n", (int)info_->format);

    IO* io = c64_->io();
    size_t height = std::min(info_->size->height, io->h());
    size_t width = std::min(info_->size->width, io->w());

    // RGB565
    const uint8_t* src = (const uint8_t*)io->display_base_addr();
    uint8_t* dst = (uint8_t*)buffer_;
    for (size_t y=0; y < height; y++) {
      uint8_t* dst_row = (uint8_t*)(dst + (y * info_->row_bytes));
      const uint8_t* src_row = src + (y * (2*io->w()));
      memcpy(dst_row, src_row, 2 * width);
    }

    /*    
    // RGB332
    const uint8_t* src = io->display_base_addr();
    uint8_t* dst = (uint8_t*)buffer_;
    for (size_t y=0; y < height; y++) {
      uint8_t* dst_row = dst + (y * info_->row_bytes);
      const uint8_t* src_row = src + (y * io->w());

      for (size_t x=0; x< width; x++ ) {
        *dst_row++ = *src_row++;
      }
    }
    */

    c64_->emscripten_loop();
    frame_buffer_->Flush([this]() { Draw(); });
  }

  private:
    class CallCreateSurface {
      public:
        explicit CallCreateSurface(EmudoreApp* app) : app_(app) {}

      void Run(mojo::InterfaceHandle<mojo::Framebuffer> frame_buffer,
               mojo::FramebufferInfoPtr info) const {
        app_->CreateSurface(std::move(frame_buffer), std::move(info));
      }

     private:
      EmudoreApp* app_;
  };

  C64* c64_;
  uintptr_t buffer_ = 0;
  mojo::FramebufferProviderPtr provider_;
  mojo::FramebufferPtr frame_buffer_;
  mojo::FramebufferInfoPtr info_;  

  MOJO_DISALLOW_COPY_AND_ASSIGN(EmudoreApp);
};

}  // namespace

MojoResult MojoMain(MojoHandle request) {
  EmudoreApp app;
  return mojo::RunApplication(request, &app);
}
