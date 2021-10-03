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

#pragma once

#include "Kraken/Microsoft/UIResponder.h"
#include "MainPage.g.h"

namespace winrt::kraken::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();

    void TitleBarLayoutEvent(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar const& sender, IInspectable const& event);
    void TitleBarVisibilityEvent(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar const& sender, IInspectable const& event);
    void WindowActivatedEvent(IInspectable const& sender, Windows::UI::Core::WindowActivatedEventArgs const& event);
    void NavigationViewControl_DisplayModeChanged(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewDisplayModeChangedEventArgs const& event);

    winrt::event<EventHandler<kraken::Event>> titleBarLayoutDidChangeEvent;
  };
}  // namespace winrt::kraken::implementation


namespace winrt::kraken::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {};
}  // namespace winrt::kraken::factory_implementation
