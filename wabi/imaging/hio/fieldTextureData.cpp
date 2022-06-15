//
// Copyright 2021 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "wabi/imaging/hio/fieldTextureData.h"

#include "wabi/imaging/hio/debugCodes.h"
#include "wabi/imaging/hio/rankedTypeMap.h"

#include "wabi/usd/ar/resolver.h"

#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/token.h"
#include "wabi/base/tf/type.h"
#include "wabi/base/tf/staticData.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_ENV_SETTING(HIO_FIELD_TEXTURE_DATA_PLUGIN_RESTRICTION, "",
      "Restricts HioFieldTextureData plugin loading to the specified plugin");

namespace
{

  class _FieldTextureDataFactoryRegistry
  {
   public:

    _FieldTextureDataFactoryRegistry();

    HioFieldTextureDataFactoryBase const *GetFactory(std::string const &filePath) const;

   private:

    HioRankedTypeMap _typeMap;
  };

  _FieldTextureDataFactoryRegistry::_FieldTextureDataFactoryRegistry()
  {
    // Register all fieldTextureData types using plugin metadata.
    _typeMap.Add(TfType::Find<HioFieldTextureData>(),
                 "fieldDataTypes",
                 HIO_DEBUG_FIELD_TEXTURE_DATA_PLUGINS,
                 TfGetEnvSetting(HIO_FIELD_TEXTURE_DATA_PLUGIN_RESTRICTION));
  }

  HioFieldTextureDataFactoryBase const *_FieldTextureDataFactoryRegistry::GetFactory(
    std::string const &filePath) const
  {
    TfToken const fileExtension(TfStringToLower(ArGetResolver().GetExtension(filePath)));

    TfType const &pluginType = _typeMap.Find(fileExtension);
    if (!pluginType) {
      // Unknown prim type.
      TF_CODING_ERROR("[PluginLoad] Unknown field data type '%s' for file '%s'\n",
                      fileExtension.GetText(),
                      filePath.c_str());
      return nullptr;
    }

    HioFieldTextureDataFactoryBase const *factory =
      pluginType.GetFactory<HioFieldTextureDataFactoryBase>();
    if (!factory) {
      TF_CODING_ERROR(
        "[PluginLoad] Cannot get factory for type '%s' "
        "for field data type '%s' for file '%s'\n",
        pluginType.GetTypeName().c_str(),
        fileExtension.GetText(),
        filePath.c_str());
      return nullptr;
    }

    return factory;
  }

  static TfStaticData<_FieldTextureDataFactoryRegistry> _factoryRegistry;

}  // end anonymous namespace

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<HioFieldTextureData>();
}

HioFieldTextureData::HioFieldTextureData() = default;

HioFieldTextureData::~HioFieldTextureData() = default;

/* static */
HioFieldTextureDataSharedPtr HioFieldTextureData::New(std::string const &filePath,
                                                      std::string const &fieldName,
                                                      int fieldIndex,
                                                      std::string const &fieldPurpose,
                                                      size_t targetMemory)
{
  HioFieldTextureDataFactoryBase const *factory = _factoryRegistry->GetFactory(filePath);
  if (!factory) {
    return nullptr;
  }

  HioFieldTextureDataSharedPtr fieldTextureData =
    factory->_New(filePath, fieldName, fieldIndex, fieldPurpose, targetMemory);

  if (!fieldTextureData) {
    TF_CODING_ERROR("Cannot get construct field texture data for file '%s'\n", filePath.c_str());

    return nullptr;
  }

  return fieldTextureData;
}


WABI_NAMESPACE_END
