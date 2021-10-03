/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2021, Wabi.
 */

/**
 * @file
 * Microsoft.
 * KrakenRT.
 */

#include "pch.h"

#include "Kraken/Microsoft/App.Xaml.h"

#ifdef WITH_WINUI3
#  include "Kraken/Microsoft/MainWindow.h"
#else
#  include "Kraken/Microsoft/MainPage.h"
#endif /* WITH_WINUI3 */

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Media;

#ifdef WITH_WINUI3
using namespace Microsoft::UI::Xaml::Navigation;
#endif /* WITH_WINUI3 */

using namespace kraken;
using namespace kraken::implementation;


App::App()
{
  InitializeComponent();

  

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException([this](IInspectable const &, UnhandledExceptionEventArgs const &e) {
    if (IsDebuggerPresent())
    {
      auto errorMessage = e.Message();
      __debugbreak();
    }
  });
#endif
}


void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &args)
{
  Windows::UI::Xaml::Window window = Windows::UI::Xaml::Window::Current();
  Windows::UI::Xaml::Controls::Frame frame{ nullptr };

  auto content = window.Content();
  if(content) {
    frame = content.try_as<Windows::UI::Xaml::Controls::Frame>();
  }

  if(frame == nullptr) {
    frame = Windows::UI::Xaml::Controls::Frame();
  }

  if(args.PrelaunchActivated() == false) {

    if(frame.Content() == nullptr) {
      frame.Navigate(xaml_typename<kraken::MainPage>(), box_value(args.Arguments()));
    }
    
    window.Content(frame);
    window.Activate();
  }
}
