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

#include "pch.h"

#include "UIKit.UIEvent.g.h"

namespace winrt::Kraken::UIKit::implementation
{
  struct UIEvent : UIEventT<UIEvent>
  {
    UIEvent() = default();
    UIEvent(winrt::CoreApplicationViewTitleBar const &titleBar);
    UIEvent(winrt::NavigationView const &view);
    UIEvent(IInspectable const &window);

    winrt::CoreApplicationViewTitleBar TitleBar();
    winrt::NavigationView NavigationView();
    IInspectable Window();

   private:

    winrt::CoreApplicationViewTitleBar m_titleBar{nullptr};
    winrt::NavigationView m_navigationView{nullptr};
    IInspectable m_window{nullptr};
  };
}  // namespace winrt::Kraken::UIKit::implementation


namespace winrt::Kraken::UIKit::factory_implementation
{
  struct UIEvent : UIEventT<UIEvent, implementation::UIEvent>
  {};
}  // namespace winrt::Kraken::UIKit::factory_implementation
