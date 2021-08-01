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
 * Creator.
 * Creating Chaos.
 */

#pragma once

#include <functional>
#include <map>
#include <vector>
#include <unknwn.h>

#include "creator_xaml_typeinfo.h"

#include "winrt/Windows.UI.Xaml.Markup.h"

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Markup
{
    struct IXamlMetadataProvider;
}

WINRT_EXPORT namespace winrt::Kraken
{
    struct XamlMetaDataProvider;
}

namespace winrt::impl
{
    template <> struct category<winrt::Kraken::XamlMetaDataProvider>{ using type = class_category; };
    template <> inline constexpr auto& name_v<winrt::Kraken::XamlMetaDataProvider> = L"Kraken.XamlMetaDataProvider";
    template <> struct default_interface<winrt::Kraken::XamlMetaDataProvider>{ using type = winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider; };
}

WINRT_EXPORT namespace winrt::Kraken
{
    struct __declspec(empty_bases) XamlMetaDataProvider : winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider
    {
        XamlMetaDataProvider(std::nullptr_t) noexcept {}
        XamlMetaDataProvider(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider(ptr, take_ownership_from_abi) {}
        XamlMetaDataProvider();
        XamlMetaDataProvider(XamlMetaDataProvider const&) noexcept = default;
        XamlMetaDataProvider(XamlMetaDataProvider&&) noexcept = default;
        XamlMetaDataProvider& operator=(XamlMetaDataProvider const&) & noexcept = default;
        XamlMetaDataProvider& operator=(XamlMetaDataProvider&&) & noexcept = default;
    };
}

namespace winrt::Kraken::implementation
{
    template <typename D, typename... I>
    struct __declspec(empty_bases) XamlMetaDataProvider_base : implements<D, Kraken::XamlMetaDataProvider, I...>
    {
        using base_type = XamlMetaDataProvider_base;
        using class_type = Kraken::XamlMetaDataProvider;
        using implements_type = typename XamlMetaDataProvider_base::implements_type;
        using implements_type::implements_type;
        
        hstring GetRuntimeClassName() const
        {
            return L"Kraken.XamlMetaDataProvider";
        }
    };
}

namespace winrt::Kraken::factory_implementation
{
  template <typename D, typename T, typename... I>
  struct __declspec(empty_bases) XamlMetaDataProviderT : implements<D, winrt::Windows::Foundation::IActivationFactory, I...>
  {
      using instance_type = Kraken::XamlMetaDataProvider;
  
      hstring GetRuntimeClassName() const
      {
          return L"Kraken.XamlMetaDataProvider";
      }
      auto ActivateInstance() const
      {
          return make<T>();
      }
  };
}

namespace winrt::Kraken::implementation
{
    template <typename D, typename... I>
    using XamlMetaDataProviderT = XamlMetaDataProvider_base<D, I...>;
}

namespace winrt::Kraken::implementation
{
    using IXamlMember = ::winrt::Windows::UI::Xaml::Markup::IXamlMember;
    using IXamlType = ::winrt::Windows::UI::Xaml::Markup::IXamlType;

    struct XamlMetaDataProvider : public ::winrt::Kraken::implementation::XamlMetaDataProvider_base<XamlMetaDataProvider>
    {
        IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const& type);
        IXamlType GetXamlType(::winrt::hstring const& fullName);
        ::winrt::com_array<::winrt::Windows::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

    private:
        std::shared_ptr<XamlTypeInfoProvider> _provider;
        std::shared_ptr<XamlTypeInfoProvider> Provider();
    };
}

namespace winrt::Kraken::factory_implementation
{
    struct XamlMetaDataProvider : XamlMetaDataProviderT<XamlMetaDataProvider, implementation::XamlMetaDataProvider>
    {
    };
}

namespace winrt::Kraken::implementation {

  using DataContextChangedEventArgs = ::winrt::Windows::UI::Xaml::DataContextChangedEventArgs;
  using DependencyObject = ::winrt::Windows::UI::Xaml::DependencyObject;
  using DependencyProperty = ::winrt::Windows::UI::Xaml::DependencyProperty;
  using FrameworkElement = ::winrt::Windows::UI::Xaml::FrameworkElement;
  using IInspectable = ::winrt::Windows::Foundation::IInspectable;
  using INotifyCollectionChanged = ::winrt::Windows::UI::Xaml::Interop::INotifyCollectionChanged;
  using INotifyPropertyChanged = ::winrt::Windows::UI::Xaml::Data::INotifyPropertyChanged;
  using PropertyChangedEventArgs = ::winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs;
  using NotifyCollectionChangedEventArgs = ::winrt::Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs;
  using ContainerContentChangingEventArgs = ::winrt::Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs;
  
  struct XamlBindings;
  
  struct IXamlBindings
  {
      virtual ~IXamlBindings() {};
      virtual bool IsInitialized() = 0;
      virtual void Update() = 0;
      virtual bool SetDataRoot(IInspectable const& data) = 0;
      virtual void StopTracking() = 0;
      virtual void Connect(int connectionId, IInspectable const& target) = 0;
      virtual void Recycle() = 0;
      virtual void ProcessBindings(IInspectable const& item, int itemIndex, int phase, int32_t& nextPhase) = 0;
      virtual void SubscribeForDataContextChanged(FrameworkElement const& object, XamlBindings& handler) = 0;
      virtual void DisconnectUnloadedObject(int connectionId) = 0;
  };
  
  struct IXamlBindingTracking
  {
      virtual void PropertyChanged(IInspectable const& sender, PropertyChangedEventArgs const& e) = 0;
      virtual void CollectionChanged(IInspectable const& sender, NotifyCollectionChangedEventArgs const& e) = 0;
      virtual void DependencyPropertyChanged(DependencyObject const& sender, DependencyProperty const& prop) = 0;
      virtual void VectorChanged(IInspectable const& sender, ::winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& e) = 0;
      virtual void MapChanged(IInspectable const& sender, ::winrt::Windows::Foundation::Collections::IMapChangedEventArgs<::winrt::hstring> const& e) = 0;
  };
  
  struct XamlBindings : public winrt::implements<XamlBindings,
      ::winrt::Windows::UI::Xaml::IDataTemplateExtension,
      ::winrt::Windows::UI::Xaml::Markup::IComponentConnector,
      ::winrt::Windows::UI::Xaml::Markup::IDataTemplateComponent>
  {
      XamlBindings(std::unique_ptr<IXamlBindings>&& pBindings);
  
      // IComponentConnector
      void Connect(int connectionId, IInspectable const& target);
  
      // IDataTemplateComponent
      virtual void ProcessBindings(IInspectable const& item, int itemIndex, int phase, int32_t& nextPhase);
      virtual void Recycle();
  
      // IDataTemplateExtension
      bool ProcessBinding(uint32_t phase);
      int ProcessBindings(ContainerContentChangingEventArgs const& args);
      void ResetTemplate();
  
      void Initialize();
      void Update();
      void StopTracking();
      void Loading(FrameworkElement const& src, IInspectable const& data);
      void DataContextChanged(FrameworkElement const& sender, DataContextChangedEventArgs const& args);
      void SubscribeForDataContextChanged(FrameworkElement const& object);
      virtual void DisconnectUnloadedObject(int connectionId);
  
  private:
      std::unique_ptr<IXamlBindings> _pBindings;
  };
  
  template <typename TBindingsTracking>
  struct XamlBindingsBase : public IXamlBindings
  {
  protected:
      bool _isInitialized = false;
      std::shared_ptr<TBindingsTracking> _bindingsTracking;
      winrt::event_token _dataContextChangedToken {};
      static const int NOT_PHASED = (1 << 31);
      static const int DATA_CHANGED = (1 << 30);
  
  protected:
      XamlBindingsBase() = default;
  
      virtual ~XamlBindingsBase()
      {
          if (_bindingsTracking)
          {
              _bindingsTracking->SetListener(nullptr);
              _bindingsTracking.reset();
          }
      }
  
      virtual void ReleaseAllListeners()
      {
          // Overridden in the binding class as needed.
      }
  
  public:
      void InitializeTracking(IXamlBindingTracking* pBindingsTracking)
      {
          _bindingsTracking = std::make_shared<TBindingsTracking>();
          _bindingsTracking->SetListener(pBindingsTracking);
      }
  
      virtual void StopTracking() override
      {
          ReleaseAllListeners();
          this->_isInitialized = false;
      }
  
      virtual bool IsInitialized() override
      {
          return this->_isInitialized;
      }
  
      void SubscribeForDataContextChanged(FrameworkElement const& object, XamlBindings& handler) override
      {
          this->_dataContextChangedToken = object.DataContextChanged({ &handler, &XamlBindings::DataContextChanged });
      }
  
      virtual void Recycle() override
      {
          // Overridden in the binding class as needed.
      }
  
      virtual void ProcessBindings(IInspectable const&, int, int, int32_t& nextPhase) override
      {
          // Overridden in the binding class as needed.
          nextPhase = -1;
      }
  };
  
  struct XamlBindingTrackingBase
  {
      XamlBindingTrackingBase();
      void SetListener(IXamlBindingTracking* pBindings);
      
      // Event handlers
      void PropertyChanged(IInspectable const& sender, PropertyChangedEventArgs const& e);
      void CollectionChanged(IInspectable const& sender, NotifyCollectionChangedEventArgs const& e);
      void DependencyPropertyChanged(DependencyObject const& sender, DependencyProperty const& prop);
      void VectorChanged(IInspectable const& sender, ::winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& e);
      void MapChanged(IInspectable const& sender, ::winrt::Windows::Foundation::Collections::IMapChangedEventArgs<::winrt::hstring> const& e);
  
      // Listener update functions
      void UpdatePropertyChangedListener(INotifyPropertyChanged const& obj, INotifyPropertyChanged& cache, ::winrt::event_token& token);
      void UpdatePropertyChangedListener(INotifyPropertyChanged const& obj, ::winrt::weak_ref<::winrt::Windows::UI::Xaml::Data::INotifyPropertyChanged>& cache, ::winrt::event_token& token);
      void UpdateCollectionChangedListener(INotifyCollectionChanged const& obj, INotifyCollectionChanged& cache, ::winrt::event_token& token);
      void UpdateDependencyPropertyChangedListener(DependencyObject const& obj, DependencyProperty const& property, DependencyObject&  cache, int64_t& token);
      void UpdateDependencyPropertyChangedListener(DependencyObject const& obj, DependencyProperty const& property, ::winrt::weak_ref<::winrt::Windows::UI::Xaml::DependencyObject>& cache, int64_t& token);
  
  private:
      IXamlBindingTracking* _pBindingsTrackingWeakRef{nullptr};
  };
  
  template <typename T>
  struct ResolveHelper
  {
      static T Resolve(::winrt::weak_ref<T> const& wr)
      {
          return wr.get();
      }
  };
  
  template<>
  struct ResolveHelper<::winrt::hstring>
  {
      using ResolveType = ::winrt::Windows::Foundation::IReference<::winrt::hstring>;
      static ::winrt::hstring Resolve(::winrt::weak_ref<ResolveType> const& wr)
      {
          return wr.get().Value();
      }
  };
  
  template<typename T, typename TBindingsTracking> 
  struct ReferenceTypeXamlBindings : public XamlBindingsBase<TBindingsTracking>
  {
  protected:
      ReferenceTypeXamlBindings() {}
  
      virtual void Update_(T, int)
      {
          // Overridden in the binding class as needed.
      }
  
  public:
      T GetDataRoot()
      {
          return ResolveHelper<T>::Resolve(this->_dataRoot);
      }
  
      bool SetDataRoot(IInspectable const& data) override
      {
          if (data)
          {
              this->_dataRoot = ::winrt::unbox_value<T>(data);
              return true;
          }
          return false;
      }
  
      virtual void Update() override
      {
          this->Update_(this->GetDataRoot(), this->NOT_PHASED);
          this->_isInitialized = true;
      }
  
  private:
          ::winrt::weak_ref<T> _dataRoot;
  };
  
  template<typename T, typename TBindingsTracking> 
  struct ValueTypeXamlBindings : public XamlBindingsBase<TBindingsTracking>
  {
  protected:
      ValueTypeXamlBindings() {}
  
      virtual void Update_(T, int)
      {
          // Overridden in the binding class as needed.
      }
  
  public:
      T GetDataRoot()
      {
          return this->_dataRoot;
      }
  
      bool SetDataRoot(IInspectable const& data) override
      {
          if (data)
          {
              this->_dataRoot = ::winrt::unbox_value<T>(data);
              return true;
          }
          return false;
      }
  
      virtual void Update() override
      {
          this->Update_(this->GetDataRoot(), this->NOT_PHASED);
          this->_isInitialized = true;
      }
  
  private:
      T _dataRoot;
  };
}
